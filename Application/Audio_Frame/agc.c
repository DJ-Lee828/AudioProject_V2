/*
 * agc.c
 *
 *  Created on: 2026. 5. 24.
 *      Author: ADJ
 */

#include "agc.h"
#include "main.h"

#define AGC_TARGET		20000.0f
#define AGC_ALPHA		0.002f				// AGC 반응성 값
#define AGC_MIN			0.5f
#define AGC_MAX			99.99f

extern volatile uint8_t agcRun;				// 게인 갱신 플래그

static float32_t s_avgPeak = AGC_TARGET;	// 최근 입력의 평균 peak
static float32_t s_gain = 1.0f;				// 현재 AGC 게인

static void AGC_Scale(int16_t *src, int16_t *dst, uint32_t count, float32_t gain)
{
	/*
	 * rm_scale_q15()는 float gain을 직접 받지 않고 이런 구조로 받음
	 * 출력 = 입력 × scaleFract × 2^shift
	 *
	 * gain이 1보다 크거나 거의 1이면, Q15 fraction으로 바로 표현하기 어려움
	 * gain이 1 이상이면 0.5배씩 줄이고, 대신 shift를 올림
	 */

	int8_t shift = 0;

	while (gain > 0.999f)
	{
		gain *= 0.5f;
		shift++;
	}

	q15_t scale = gain * 32768.0f;

	arm_scale_q15(
		src,
		scale,
		shift,
		dst,
		count);
}

void AGC_Run(int16_t *src, int16_t *dst, uint32_t count)
{
	q15_t peak;			// 현재 블록에서 가장 큰 절댓값 저장
	uint32_t index;		// peak가 발견된 인덱스 Unused

	arm_absmax_q15(src, count, &peak, &index);

	if (agcRun && peak)	// AGC 학습 플래그 및 무음 검사
	{
		s_avgPeak += AGC_ALPHA * (peak - s_avgPeak);	// 평균 peak를 천천히 따라가게 함
		s_gain = AGC_TARGET / s_avgPeak;				// AGC 타겟을 peak로 나눠 게인 비를 구함

		if (s_gain < AGC_MIN) s_gain = AGC_MIN;
		if (s_gain > AGC_MAX) s_gain = AGC_MAX;
	}

	AGC_Scale(src, dst, count, s_gain);
}

float AGC_GetGain(void)
{
	return s_gain;
}
