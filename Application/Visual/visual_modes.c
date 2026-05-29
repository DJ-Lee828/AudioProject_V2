/*
 * visual_modes.c
 *
 *  Created on: 2026. 5. 21.
 *      Author: nugur
 */

#include "visual_renderer.h"
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
		s_gammaTable[i] = powf(x, 2.2f) * 210;
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

// ==================================================
// FULL MIRROR
// ==================================================
void VisualModes_DrawMirror_Full(const float *trail)
{
    memset(s_frame, 0, sizeof(s_frame));

    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        int h = (int)(trail[x] + 0.5f);
        if (h > MATRIX_HEIGHT) h = MATRIX_HEIGHT;
        if (h < 0) h = 0;

        float r, g, b;
        VisualTheme_GetMirrorColor(s_mirrorTheme, 1.0f, &r, &g, &b);
        // ★ t=1 고정 → 단색화

        uint8_t R = (uint8_t)r;
        uint8_t G = (uint8_t)g;
        uint8_t B = (uint8_t)b;

        uint8_t cr = s_gammaTable[R];
        uint8_t cg = s_gammaTable[G];
        uint8_t cb = s_gammaTable[B];

        for (int y = 0; y < h; y++)
        {
            int top = y;
            int bottom = (MATRIX_HEIGHT - 1) - y;

            s_frame[top][x][0] = cr;
            s_frame[top][x][1] = cg;
            s_frame[top][x][2] = cb;

            s_frame[bottom][x][0] = cr;
            s_frame[bottom][x][1] = cg;
            s_frame[bottom][x][2] = cb;
        }
    }
}

// ==================================================
// CENTER MIRROR
// ==================================================
void VisualModes_DrawMirror_Center(const float *trail)
{
    memset(s_frame, 0, sizeof(s_frame));

    const int halfH = MATRIX_HEIGHT / 2;

    float r, g, b;
    VisualTheme_GetMirrorColor(s_mirrorTheme, 1.0f, &r, &g, &b);
    // ★ 완전 단색

    uint8_t cr = s_gammaTable[(uint8_t)r];
    uint8_t cg = s_gammaTable[(uint8_t)g];
    uint8_t cb = s_gammaTable[(uint8_t)b];

    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        int h = (int)(trail[x] + 0.5f);
        if (h > halfH) h = halfH;
        if (h < 0) h = 0;

        for (int y = 0; y < h; y++)
        {
            int y0 = halfH - 1 - y;
            int y1 = halfH + y;

            if (y0 >= 0)
            {
                s_frame[y0][x][0] = cr;
                s_frame[y0][x][1] = cg;
                s_frame[y0][x][2] = cb;
            }

            if (y1 < MATRIX_HEIGHT)
            {
                s_frame[y1][x][0] = cr;
                s_frame[y1][x][1] = cg;
                s_frame[y1][x][2] = cb;
            }
        }
    }
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
void VisualModes_DrawPulse(const float *trail)
{
    memset(s_frame, 0, sizeof(s_frame));

    static float t_global = 0.0f;

    // 내부에서 energy 계산 (FFT 기반 or 더미)
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
        energy += trail[i];

    energy /= MATRIX_WIDTH;
    energy = energy / (float)MATRIX_HEIGHT; // normalize

    float speed = 0.02f + energy * 0.05f;

    t_global += speed;

    float pulse = 0.5f + 0.5f * sinf(t_global);
    pulse *= pulse;

    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        int h = (int)(trail[x] + 0.5f);
        if (h > MATRIX_HEIGHT) h = MATRIX_HEIGHT;

        for (int y = 0; y < h; y++)
        {
            float t = (float)y / (float)(MATRIX_HEIGHT - 1);

            float r, g, b;
            VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

            int R = (int)(r * pulse);
            int G = (int)(g * pulse);
            int B = (int)(b * pulse);

            if (R > 255) R = 255;
            if (G > 255) G = 255;
            if (B > 255) B = 255;

            s_frame[y][x][0] = s_gammaTable[R];
            s_frame[y][x][1] = s_gammaTable[G];
            s_frame[y][x][2] = s_gammaTable[B];
        }
    }
}
void VisualModes_DrawGridBreath(const float *trail, float time)
{
	memset(s_frame, 0, sizeof(s_frame));

	    float breath = 0.5f + 0.5f * sinf(time * 1.5f);
	    breath *= breath;

	    for (int y = 0; y < MATRIX_HEIGHT; y++)
	    {
	        for (int x = 0; x < MATRIX_WIDTH; x++)
	        {
	            float noise =
	                sinf(x * 0.8f + time) *
	                cosf(y * 0.7f + time);

	            float v = (noise + 1.0f) * 0.5f * breath;

	            float r, g, b;
	            VisualTheme_GetColor(s_spectrumTheme, v, &r, &g, &b);

	            r *= 0.6f;
	            g *= 0.6f;
	            b *= 0.6f;

	            uint8_t rr = s_gammaTable[(uint8_t)r];
	            uint8_t gg = s_gammaTable[(uint8_t)g];
	            uint8_t bb = s_gammaTable[(uint8_t)b];

	            s_frame[y][x][0] = rr;
	            s_frame[y][x][1] = gg;
	            s_frame[y][x][2] = bb;
        }
    }
}

void VisualModes_DrawRotatingField(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    float cx = MATRIX_WIDTH * 0.5f;
    float cy = MATRIX_HEIGHT * 0.5f;

    float angle = time * 0.8f;

    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            float dx = x - cx;
            float dy = y - cy;

            float rx = dx * cosf(angle) - dy * sinf(angle);
            float ry = dx * sinf(angle) + dy * cosf(angle);

            float v = sinf(rx * 0.3f) + cosf(ry * 0.3f);
            v = (v + 2.0f) / 4.0f;

            float r, g, b;
            VisualTheme_GetColor(s_spectrumTheme, v, &r, &g, &b);

            uint8_t rr = s_gammaTable[(uint8_t)r];
            uint8_t gg = s_gammaTable[(uint8_t)g];
            uint8_t bb = s_gammaTable[(uint8_t)b];

            s_frame[y][x][0] = rr;
            s_frame[y][x][1] = gg;
            s_frame[y][x][2] = bb;
        }
    }
}

void VisualModes_DrawSparkNoise(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        float base = trail[x];

        float wave =
            sinf(time * 2.0f + x * 0.35f) * 2.5f +
            sinf(time * 1.1f + x * 0.12f) * 1.5f;

        float heightF = base + wave;

        if (heightF < 0) heightF = 0;
        if (heightF > MATRIX_HEIGHT) heightF = MATRIX_HEIGHT;

        int height = (int)(heightF + 0.5f);

        for (int y = 0; y < height; y++)
        {
            float t = (float)y / (float)(MATRIX_HEIGHT - 1);

            float r, g, b;
            VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

            // =========================================
            // NOISE (스파이크 약하게)
            // =========================================
            float n =
                sinf(x * 12.9898f + y * 78.233f + time * 6.0f);

            n = n - floorf(n);

            float spark = 0.0f;
            if (n > 0.88f)   // ← 확률 낮춤 (덜 튐)
            {
                spark = 2.0f; // ← 강도 감소
            }

            // =========================================
            // ENERGY
            // =========================================
            float energy = 0.6f + 0.4f * sinf(time * 1.8f);
            energy *= energy;

            // =========================================
            // BASE LIGHT (전체 밝기 증가)
            // =========================================
            float baseLight = 0.72f;   // ↑ 전체 밝기 크게 증가

            float base = baseLight * (0.75f + 0.25f * energy);

            // =========================================
            // FINAL INTENSITY
            // =========================================
            float intensity = base + (spark * energy * 0.6f); // ← 스파크 영향 감소

            // =========================================
            // APPLY COLOR
            // =========================================
            r *= intensity * 0.90f;
            g *= intensity * 0.90f;
            b *= intensity * 0.90f;

            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            uint8_t rr = s_gammaTable[(uint8_t)r];
            uint8_t gg = s_gammaTable[(uint8_t)g];
            uint8_t bb = s_gammaTable[(uint8_t)b];

            s_frame[y][x][0] = rr;
            s_frame[y][x][1] = gg;
            s_frame[y][x][2] = bb;

            // =========================================
            // SPARK BLEED (약하게 유지)
            // =========================================
            if (spark > 0.0f && y + 1 < MATRIX_HEIGHT)
            {
                s_frame[y + 1][x][0] = rr;
                s_frame[y + 1][x][1] = gg;
                s_frame[y + 1][x][2] = bb;
            }
        }
    }
}

// ======================================================
// WATERUP (FIXED COLOR)
// ======================================================
void VisualModes_DrawWaterup(const float *trail)
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
// GET FRAME
// ======================================================
const uint8_t *VisualModes_GetFrame(void)
{
	return &s_frame[0][0][0];
}
