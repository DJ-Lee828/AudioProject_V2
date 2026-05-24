/*
 * visual_modes.c
 *
 *  Created on: 2026. 5. 21.
 *      Author: nugur
 */

#include "visual_modes.h"
#include "visual_theme.h"
#include <string.h>
#include <math.h>

// ======================================================
// FRAME BUFFER
// ======================================================
static uint8_t s_frame[MATRIX_HEIGHT][MATRIX_WIDTH][3];
static uint8_t s_gammaTable[256];

// ======================================================
// GAMMA
// ======================================================
static void BuildGamma(void)
{
	for (int i = 0; i < 256; i++)
    {
		float x = (float)i / 255.0f;
		s_gammaTable[i] = powf(x, 1.5f) * 90; // 1.2 , 100
    }
}

// ======================================================
// INIT
// ======================================================
void VisualModes_Init(void)
{
	memset(s_frame, 0, sizeof(s_frame));
	BuildGamma();
}

// ======================================================
// CLEAR
// ======================================================
void VisualModes_Clear(void)
{
	memset(s_frame, 0, sizeof(s_frame));
}

// ======================================================
// SPECTRUM CORE (FIXED)
// ======================================================
static void DrawSpectrumTheme(uint8_t theme, const float *trail, const float *peakHold)
{
	memset(s_frame, 0, sizeof(s_frame));

	for (int x = 0; x < MATRIX_WIDTH; x++)
	{
		// =====================================================
		// BAR HEIGHT
		// =====================================================
		int h = (int)(trail[x] + 0.5f);
		if (h < 0) h = 0;
		if (h > MATRIX_HEIGHT) h = MATRIX_HEIGHT;

		// =====================================================
		// DRAW BAR
		// =====================================================
		for (int y = 0; y < h; y++)
		{
			float t =
				(float)y /
				(float)(MATRIX_HEIGHT - 1);

			float r, g, b;

			VisualTheme_GetColor(
				theme,
				t,
				&r, &g, &b);

			// glow 제거된 순수 fill 방식 (예전 스타일 유지)

			s_frame[y][x][0] =
					s_gammaTable[(uint8_t)r];

			s_frame[y][x][1] =
					s_gammaTable[(uint8_t)g];

			s_frame[y][x][2] =
					s_gammaTable[(uint8_t)b];
		}

		// =====================================================
		// PEAK HOLD (SINGLE PIXEL)
		// =====================================================
		int p = (int)(peakHold[x] + 0.5f);

		if (p >= 0 && p < MATRIX_HEIGHT)
		{
			float pr, pg, pb;

			VisualTheme_GetPeakColor(
				theme,
				&pr, &pg, &pb);

			s_frame[p][x][0] =
					s_gammaTable[(uint8_t)pr];

			s_frame[p][x][1] =
					s_gammaTable[(uint8_t)pg];

			s_frame[p][x][2] =
					s_gammaTable[(uint8_t)pb];
		}
	}
}

// ======================================================
// SPECTRUM 1~6
// ======================================================
void VisualModes_DrawSpectrum1(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(0, trail, peakHold);
}

void VisualModes_DrawSpectrum2(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(1, trail, peakHold);
}

void VisualModes_DrawSpectrum3(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(2, trail, peakHold);
}

void VisualModes_DrawSpectrum4(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(3, trail, peakHold);
}

void VisualModes_DrawSpectrum5(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(4, trail, peakHold);
}

void VisualModes_DrawSpectrum6(const float *trail, const float *peakHold)
{
	DrawSpectrumTheme(5, trail, peakHold);
}

void VisualModes_DrawMirror_Full(const float *trail, const float *peakHold)
{
	memset(s_frame, 0, sizeof(s_frame));

	for (int x = 0; x < MATRIX_WIDTH; x++)
	{
		int h = (int)(trail[x] + 0.5f);
		if (h > MATRIX_HEIGHT) h = MATRIX_HEIGHT;

		// ==================================================
		// BAR (FULL MIRROR)
		// ==================================================
		for (int y = 0; y < h; y++)
		{
			float t = (float)y / (float)(MATRIX_HEIGHT - 1);

			float r, g, b;
			VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

			uint8_t R = s_gammaTable[(uint8_t)r];
			uint8_t G = s_gammaTable[(uint8_t)g];
			uint8_t B = s_gammaTable[(uint8_t)b];

			int top    = y;
			int bottom = (MATRIX_HEIGHT - 1) - y;

			s_frame[top][x][0] = R;
			s_frame[top][x][1] = G;
			s_frame[top][x][2] = B;

			s_frame[bottom][x][0] = R;
			s_frame[bottom][x][1] = G;
			s_frame[bottom][x][2] = B;
		}

		// ==================================================
		// PEAK HOLD
		// ==================================================
		int p = (int)(peakHold[x] + 0.5f);
		if (p >= 0 && p < MATRIX_HEIGHT)
		{
			float pr, pg, pb;
			VisualTheme_GetPeakColor(s_spectrumTheme, &pr, &pg, &pb);

			uint8_t R = s_gammaTable[(uint8_t)pr];
			uint8_t G = s_gammaTable[(uint8_t)pg];
			uint8_t B = s_gammaTable[(uint8_t)pb];

			int top    = p;
			int bottom = (MATRIX_HEIGHT - 1) - p;

			s_frame[top][x][0] = R;
			s_frame[top][x][1] = G;
			s_frame[top][x][2] = B;

			s_frame[bottom][x][0] = R;
			s_frame[bottom][x][1] = G;
			s_frame[bottom][x][2] = B;
		}
	}
}

void VisualModes_DrawMirror_Center(const float *trail, const float *peakHold)
{
	memset(s_frame, 0, sizeof(s_frame));

	const int halfH = MATRIX_HEIGHT / 2;

	for (int x = 0; x < MATRIX_WIDTH; x++)
	{
		int h = (int)(trail[x] + 0.5f);
		if (h > halfH) h = halfH;
		if (h < 0) h = 0;

		for (int y = 0; y < h; y++)
		{
			float t = (float)y / (float)(halfH - 1);

			float r, g, b;
			VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

			uint8_t R = s_gammaTable[(uint8_t)r];
			uint8_t G = s_gammaTable[(uint8_t)g];
			uint8_t B = s_gammaTable[(uint8_t)b];

			int top = halfH - 1 - y;
			int bottom = halfH + y;

			// =========================
			// 충돌 방지 핵심
			// =========================

			s_frame[top][x][0] = R;
			s_frame[top][x][1] = G;
			s_frame[top][x][2] = B;

			s_frame[bottom][x][0] = R;
			s_frame[bottom][x][1] = G;
			s_frame[bottom][x][2] = B;
		}

		// =========================
		// PEAK (중앙 기준 단일 write)
		// =========================

		int p = (int)(peakHold[x] + 0.5f);
		if (p >= halfH) p = halfH - 1;
		if (p < 0) p = 0;

		float pr, pg, pb;
		VisualTheme_GetPeakColor(s_spectrumTheme, &pr, &pg, &pb);

		uint8_t R = s_gammaTable[(uint8_t)pr];
		uint8_t G = s_gammaTable[(uint8_t)pg];
		uint8_t B = s_gammaTable[(uint8_t)pb];

		int top = halfH - 1 - p;
		int bottom = halfH + p;

		s_frame[top][x][0] = R;
		s_frame[top][x][1] = G;
		s_frame[top][x][2] = B;

		s_frame[bottom][x][0] = R;
		s_frame[bottom][x][1] = G;
		s_frame[bottom][x][2] = B;
	}
}

// ======================================================
// WATERFALL (FIXED COLOR)
// ======================================================
void VisualModes_DrawWaterfall(const float *trail, const float *peakHold)
{
	static uint8_t waterfall[MATRIX_HEIGHT][MATRIX_WIDTH][3];

	for (int y = MATRIX_HEIGHT - 1; y > 0; y--)
		memcpy(waterfall[y], waterfall[y - 1], MATRIX_WIDTH * 3);

	for (int x = 0; x < MATRIX_WIDTH; x++)
	{
		int h = (int)(trail[x]);

		float t = (float)h / (float)MATRIX_HEIGHT;

		float r,g,b;

		VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

		waterfall[0][x][0] = s_gammaTable[(uint8_t)r];
		waterfall[0][x][1] = s_gammaTable[(uint8_t)g];
		waterfall[0][x][2] = s_gammaTable[(uint8_t)b];
	}

	memcpy(s_frame, waterfall, sizeof(s_frame));
}

// ======================================================
// RAINBOW
// ======================================================
void VisualModes_DrawRainbow(const float *trail)
{
	memset(s_frame, 0, sizeof(s_frame));

	static float hue = 0;
	hue += 0.02f;

	for (int x = 0; x < MATRIX_WIDTH; x++)
	{
		int h = (int)trail[x];

		for (int y = 0; y < h; y++)
		{
			float t = fmodf(hue + x * 0.1f + y * 0.03f, 1.0f);

			float r, g, b;
			VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

			int yy = y;  // ★ 통일: flip 제거

			s_frame[yy][x][0] = s_gammaTable[(uint8_t)r];
			s_frame[yy][x][1] = s_gammaTable[(uint8_t)g];
			s_frame[yy][x][2] = s_gammaTable[(uint8_t)b];
		}
	}
}

// ======================================================
// GET FRAME
// ======================================================
const uint8_t *VisualModes_GetFrame(void)
{
	return &s_frame[0][0][0];
}
