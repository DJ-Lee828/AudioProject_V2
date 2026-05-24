#include <Arduino.h>
#include "BluetoothA2DPSink.h"
#include "driver/i2s.h"
#include "esp_bt.h"
#include "esp_wifi.h"

/* ============================================================
 * User config
 * ============================================================ */
#define BT_NAME					"NUCLEO_AUDIO"

#define LED_D2					2

#define I2S_PORT				I2S_NUM_0
#define I2S_BCLK				21
#define I2S_LRCK				23
#define I2S_DOUT				22

#define OUT_RATE				48000U

#define AUDIO_BLOCK_FRAMES	512U
#define AUDIO_BLOCK_SAMPLES	(AUDIO_BLOCK_FRAMES * 2U)
#define AUDIO_BLOCK_BYTES		(AUDIO_BLOCK_SAMPLES * sizeof(int16_t))
#define AUDIO_BLOCK_COUNT		8U

#define I2S_WRITE_FRAMES		256U
#define I2S_WRITE_WORDS			(I2S_WRITE_FRAMES * 2U)

#define AUDIO_TASK_STACK		4096
#define AUDIO_TASK_PRIORITY	3
#define AUDIO_TASK_CORE			1

/* ============================================================
 * Types
 * ============================================================ */
typedef struct
{
	uint16_t frames;
	uint32_t rate;
	int16_t pcm[AUDIO_BLOCK_SAMPLES];
} AudioBlock_t;

/* ============================================================
 * Globals
 * ============================================================ */
static BluetoothA2DPSink a2dp;

static AudioBlock_t blockPool[AUDIO_BLOCK_COUNT];

static QueueHandle_t freeQueue = NULL;
static QueueHandle_t readyQueue = NULL;

static volatile uint32_t btSampleRate = 44100;

static int32_t i2sTxBuf[I2S_WRITE_WORDS];
static int32_t silenceBuf[I2S_WRITE_WORDS];

/* ============================================================
 * Resampler state
 * ============================================================ */
#define RESAMPLE_WORK_FRAMES	(AUDIO_BLOCK_FRAMES + 1U)

static int16_t workBuf[RESAMPLE_WORK_FRAMES * 2U];

static bool havePrev = false;
static int16_t prevL = 0;
static int16_t prevR = 0;

static uint32_t srcPosQ16 = 0;
static uint32_t srcStepQ16 = 0;

/* ============================================================
 * Helpers
 * ============================================================ */
static int16_t Clamp16(int32_t x)
{
	if (x > 32767)
	{
		return 32767;
	}

	if (x < -32768)
	{
		return -32768;
	}

	return (int16_t)x;
}

static int32_t Pcm16ToI2s32(int16_t s)
{
	return ((int32_t)s) << 16;
}

/* ============================================================
 * I2S
 * ============================================================ */
static void I2S_Begin(void)
{
	i2s_config_t cfg = {
		.mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX),
		.sample_rate = OUT_RATE,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = 0,
		.dma_buf_count = 4,
		.dma_buf_len = 128,
		.use_apll = false,
		.tx_desc_auto_clear = true,
		.fixed_mclk = 0
	};

	i2s_pin_config_t pin = {
		.bck_io_num = I2S_BCLK,
		.ws_io_num = I2S_LRCK,
		.data_out_num = I2S_DOUT,
		.data_in_num = I2S_PIN_NO_CHANGE
	};

	i2s_driver_install(I2S_PORT, &cfg, 0, NULL);
	i2s_set_pin(I2S_PORT, &pin);
	i2s_zero_dma_buffer(I2S_PORT);

	for (uint32_t i = 0; i < I2S_WRITE_WORDS; i++)
	{
		silenceBuf[i] = 0;
	}
}

static bool I2S_Write32(const int32_t *data, size_t words)
{
	size_t bytesToWrite = words * sizeof(int32_t);
	size_t written = 0;

	i2s_write(
		I2S_PORT,
		data,
		bytesToWrite,
		&written,
		pdMS_TO_TICKS(20)
	);

	if (written == bytesToWrite)
	{
		digitalWrite(LED_D2, HIGH);
		return true;
	}

	digitalWrite(LED_D2, LOW);
	return false;
}

static void I2S_WritePcm16(const int16_t *pcm, uint16_t frames)
{
	uint16_t done = 0;

	while (done < frames)
	{
		uint16_t n = frames - done;

		if (n > I2S_WRITE_FRAMES)
		{
			n = I2S_WRITE_FRAMES;
		}

		for (uint16_t i = 0; i < n; i++)
		{
			int16_t l = pcm[(done + i) * 2U + 0U];
			int16_t r = pcm[(done + i) * 2U + 1U];

			i2sTxBuf[i * 2U + 0U] = Pcm16ToI2s32(l);
			i2sTxBuf[i * 2U + 1U] = Pcm16ToI2s32(r);
		}

		I2S_Write32(i2sTxBuf, n * 2U);

		done += n;
	}
}

static void I2S_WriteSilence(void)
{
	I2S_Write32(silenceBuf, I2S_WRITE_WORDS);
}

/* ============================================================
 * Queue
 * ============================================================ */
static void Queues_Begin(void)
{
	freeQueue = xQueueCreate(AUDIO_BLOCK_COUNT, sizeof(uint8_t));
	readyQueue = xQueueCreate(AUDIO_BLOCK_COUNT, sizeof(uint8_t));

	if (freeQueue == NULL || readyQueue == NULL)
	{
		while (1)
		{
			delay(1000);
		}
	}

	for (uint8_t i = 0; i < AUDIO_BLOCK_COUNT; i++)
	{
		xQueueSend(freeQueue, &i, portMAX_DELAY);
	}
}

static bool GetFreeBlock(uint8_t *index)
{
	return xQueueReceive(freeQueue, index, 0) == pdTRUE;
}

static void ReturnBlock(uint8_t index)
{
	xQueueSend(freeQueue, &index, 0);
}

static void DropOldestReadyBlock(void)
{
	uint8_t index;

	if (xQueueReceive(readyQueue, &index, 0) == pdTRUE)
	{
		ReturnBlock(index);
	}
}

static void FlushReadyQueue(void)
{
	uint8_t index;

	while (xQueueReceive(readyQueue, &index, 0) == pdTRUE)
	{
		ReturnBlock(index);
	}
}

/* ============================================================
 * Resampler
 * ============================================================ */
static void Resampler_Reset(uint32_t inRate)
{
	if (inRate == 0)
	{
		inRate = 44100;
	}

	havePrev = false;
	prevL = 0;
	prevR = 0;

	srcPosQ16 = 0;
	srcStepQ16 = (uint32_t)(((uint64_t)inRate << 16) / OUT_RATE);
}

static void Resampler_Write48k(const int16_t *in, uint16_t inFrames, uint32_t inRate)
{
	if (in == NULL || inFrames == 0 || inRate == 0)
	{
		return;
	}

	if (inRate == OUT_RATE)
	{
		I2S_WritePcm16(in, inFrames);
		return;
	}

	uint16_t workFrames = 0;

	if (havePrev)
	{
		workBuf[0] = prevL;
		workBuf[1] = prevR;
		workFrames = 1;
	}

	for (uint16_t i = 0; i < inFrames; i++)
	{
		workBuf[(workFrames + i) * 2U + 0U] = in[i * 2U + 0U];
		workBuf[(workFrames + i) * 2U + 1U] = in[i * 2U + 1U];
	}

	workFrames += inFrames;

	prevL = in[(inFrames - 1U) * 2U + 0U];
	prevR = in[(inFrames - 1U) * 2U + 1U];
	havePrev = true;

	if (workFrames < 2U)
	{
		return;
	}

	uint16_t outFrames = 0;

	while (((srcPosQ16 >> 16) + 1U) < workFrames)
	{
		uint32_t idx = srcPosQ16 >> 16;
		uint32_t frac = srcPosQ16 & 0xFFFFU;

		int16_t l0 = workBuf[idx * 2U + 0U];
		int16_t r0 = workBuf[idx * 2U + 1U];
		int16_t l1 = workBuf[(idx + 1U) * 2U + 0U];
		int16_t r1 = workBuf[(idx + 1U) * 2U + 1U];

		int32_t dl = (int32_t)l1 - (int32_t)l0;
		int32_t dr = (int32_t)r1 - (int32_t)r0;

		int32_t l = (int32_t)l0 + (int32_t)(((int64_t)dl * (int64_t)frac) >> 16);
		int32_t r = (int32_t)r0 + (int32_t)(((int64_t)dr * (int64_t)frac) >> 16);

		i2sTxBuf[outFrames * 2U + 0U] = Pcm16ToI2s32(Clamp16(l));
		i2sTxBuf[outFrames * 2U + 1U] = Pcm16ToI2s32(Clamp16(r));

		outFrames++;
		srcPosQ16 += srcStepQ16;

		if (outFrames >= I2S_WRITE_FRAMES)
		{
			I2S_Write32(i2sTxBuf, outFrames * 2U);
			outFrames = 0;
		}
	}

	srcPosQ16 -= ((uint32_t)(workFrames - 1U) << 16);

	if (outFrames > 0)
	{
		I2S_Write32(i2sTxBuf, outFrames * 2U);
	}
}

/* ============================================================
 * Bluetooth callbacks
 * ============================================================ */
static void OnSampleRate(uint16_t rate)
{
	if (rate == 0)
	{
		return;
	}

	btSampleRate = rate;
}

static void OnAudioData(const uint8_t *data, uint32_t len)
{
	if (data == NULL || len < 4U || (len % 4U) != 0U)
	{
		return;
	}

	const uint8_t *ptr = data;
	uint32_t bytesLeft = len;

	while (bytesLeft >= 4U)
	{
		uint16_t frames = (uint16_t)(bytesLeft / 4U);

		if (frames > AUDIO_BLOCK_FRAMES)
		{
			frames = AUDIO_BLOCK_FRAMES;
		}

		uint8_t index;

		if (!GetFreeBlock(&index))
		{
			DropOldestReadyBlock();

			if (!GetFreeBlock(&index))
			{
				return;
			}
		}

		uint32_t blockBytes = (uint32_t)frames * 4U;

		blockPool[index].frames = frames;
		blockPool[index].rate = btSampleRate;
		memcpy(blockPool[index].pcm, ptr, blockBytes);

		if (xQueueSend(readyQueue, &index, 0) != pdTRUE)
		{
			ReturnBlock(index);
			return;
		}

		ptr += blockBytes;
		bytesLeft -= blockBytes;
	}
}

/* ============================================================
 * Audio task
 * ============================================================ */
static void AudioTask(void *arg)
{
	(void)arg;

	uint32_t activeRate = 0;
	Resampler_Reset(44100);

	while (1)
	{
		uint8_t index;

		if (xQueueReceive(readyQueue, &index, pdMS_TO_TICKS(20)) != pdTRUE)
		{
			I2S_WriteSilence();
			continue;
		}

		AudioBlock_t *block = &blockPool[index];

		if (block->rate != activeRate)
		{
			activeRate = block->rate;
			Resampler_Reset(activeRate);
			FlushReadyQueue();
		}

		Resampler_Write48k(block->pcm, block->frames, block->rate);

		ReturnBlock(index);
	}
}

/* ============================================================
 * Setup
 * ============================================================ */
static void System_Begin(void)
{
	esp_wifi_stop();
	esp_wifi_deinit();

	esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
	esp_bt_sleep_disable();

	setCpuFrequencyMhz(160);

	esp_bredr_tx_power_set(ESP_PWR_LVL_N6, ESP_PWR_LVL_N6);
}

void setup()
{
	pinMode(LED_D2, OUTPUT);
	digitalWrite(LED_D2, LOW);

	System_Begin();
	Queues_Begin();
	I2S_Begin();

	xTaskCreatePinnedToCore(
		AudioTask,
		"AudioTask",
		AUDIO_TASK_STACK,
		NULL,
		AUDIO_TASK_PRIORITY,
		NULL,
		AUDIO_TASK_CORE
	);

	a2dp.set_auto_reconnect(false);
	a2dp.set_avrc_metadata_attribute_mask(0);
	a2dp.set_sample_rate_callback(OnSampleRate);
	a2dp.set_stream_reader(OnAudioData, false);
	a2dp.start(BT_NAME);
}

void loop()
{
	delay(1000);
}