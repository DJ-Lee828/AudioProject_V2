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

void VisualModes_DrawGlitchGrid(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        float base = trail[x];

        // =========================================
        // NORMAL SIGNAL
        // =========================================
        int h = (int)(base + 0.5f);
        if (h < 0) h = 0;
        if (h > MATRIX_HEIGHT) h = MATRIX_HEIGHT;

        for (int y = 0; y < h; y++)
        {
            float t = (float)y / (float)(MATRIX_HEIGHT - 1);

            float r, g, b;
            VisualTheme_GetColor(s_spectrumTheme, t, &r, &g, &b);

            // =========================================
            // GLITCH NOISE (확률 기반 붕괴)
            // =========================================
            float n =
                sinf(x * 91.17f + y * 57.31f + time * 12.0f);

            n = n - floorf(n);

            float glitch = 0.0f;

            // 낮은 확률 = 강한 깨짐
            if (n > 0.93f)
            {
                glitch = 2.8f;
            }

            // =========================================
            // ROW / COLUMN SHIFT 느낌
            // =========================================
            float shift =
                sinf(time * 4.0f + y * 0.6f) * 0.3f;

            float intensity = 0.65f + glitch + shift;

            r *= intensity;
            g *= intensity;
            b *= intensity;

            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            uint8_t rr = s_gammaTable[(uint8_t)r];
            uint8_t gg = s_gammaTable[(uint8_t)g];
            uint8_t bb = s_gammaTable[(uint8_t)b];

            // =========================================
            // RANDOM MEMORY CORRUPTION STYLE
            // =========================================
            if (glitch > 0.0f && (y % 3 == 0))
            {
                int y2 = y + (int)(sinf(time * 10.0f) * 2.0f);

                if (y2 >= 0 && y2 < MATRIX_HEIGHT)
                {
                    s_frame[y2][x][0] = rr;
                    s_frame[y2][x][1] = gg;
                    s_frame[y2][x][2] = bb;
                }
            }

            s_frame[y][x][0] = rr;
            s_frame[y][x][1] = gg;
            s_frame[y][x][2] = bb;
        }
    }
}

void VisualModes_DrawPlasma(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    // =========================================
    // AUDIO ENERGY
    // =========================================
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        energy += trail[i];
    }

    energy /= MATRIX_WIDTH;
    energy /= MATRIX_HEIGHT;

    // =========================================
    // SLOW MOTION (눈 안 아프게 튜닝)
    // =========================================
    float motion =
        0.35f +
        energy * 0.45f;

    // =========================================
    // PLASMA FIELD
    // =========================================
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            float fx = (float)x;
            float fy = (float)y;

            // =====================================
            // LARGE / SOFT WAVES
            // =====================================
            float v1 =
                sinf(
                    fx * 0.22f +
                    time * 0.45f * motion);

            float v2 =
                sinf(
                    fy * 0.22f +
                    time * 0.40f * motion);

            // =====================================
            // CENTER RADIAL WAVE
            // =====================================
            float dx =
                fx - MATRIX_WIDTH * 0.5f;

            float dy =
                fy - MATRIX_HEIGHT * 0.5f;

            float dist =
                sqrtf(dx * dx + dy * dy);

            float v3 =
                sinf(
                    dist * 0.28f -
                    time * 0.65f * motion);

            // =====================================
            // COMBINE
            // =====================================
            float plasma =
                (v1 + v2 + v3) / 3.0f;

            plasma =
                0.5f + plasma * 0.5f;

            // =====================================
            // BRIGHTNESS
            // =====================================
            float brightness =
                0.18f +
                plasma * (0.55f + energy * 0.35f);

            // =====================================
            // COLOR FLOW
            // =====================================
            float colorT =
                fmodf(
                    plasma +
                    time * 0.015f,
                    1.0f);

            float r, g, b;

            VisualTheme_GetColor(
                s_spectrumTheme,
                colorT,
                &r,
                &g,
                &b);

            // =====================================
            // APPLY BRIGHTNESS
            // =====================================
            r *= brightness;
            g *= brightness;
            b *= brightness;

            // =====================================
            // SOFT HIGHLIGHT
            // =====================================
            if (plasma > 0.82f)
            {
                r *= 1.12f;
                g *= 1.12f;
                b *= 1.12f;
            }

            // =====================================
            // CLAMP
            // =====================================
            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            // =====================================
            // GAMMA
            // =====================================
            s_frame[y][x][0] =
                s_gammaTable[(uint8_t)r];

            s_frame[y][x][1] =
                s_gammaTable[(uint8_t)g];

            s_frame[y][x][2] =
                s_gammaTable[(uint8_t)b];
        }
    }
}

void VisualModes_DrawMultiOrbit(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    // =========================================
    // ENERGY
    // =========================================
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
        energy += trail[i];

    energy /= MATRIX_WIDTH;
    energy /= MATRIX_HEIGHT;

    // =========================================
    // CENTER
    // =========================================
    float cx = MATRIX_WIDTH  * 0.5f;
    float cy = MATRIX_HEIGHT * 0.5f;

    // =========================================
    // 3 ORBIT LAYERS
    // =========================================
    for (int layer = 0; layer < 3; layer++)
    {
        int particleCount =
            10 + layer * 6;

        float baseRadius =
            2.0f + layer * 2.5f;

        float speed =
            0.7f + layer * 0.35f;

        for (int i = 0; i < particleCount; i++)
        {
            // ---------------------------------
            // orbit angle
            // ---------------------------------
            float angle =
                time * speed +
                i * (6.28318f / particleCount);

            // ---------------------------------
            // dynamic radius
            // ---------------------------------
            float radius =
                baseRadius +
                energy * (1.5f + layer);

            // slight wobble
            radius +=
                sinf(time * 1.2f + i)
                * 0.6f;

            float fx =
                cx + cosf(angle) * radius;

            float fy =
                cy + sinf(angle) * radius;

            int x = (int)(fx + 0.5f);
            int y = (int)(fy + 0.5f);

            if (x < 0 || x >= MATRIX_WIDTH)
                continue;

            if (y < 0 || y >= MATRIX_HEIGHT)
                continue;

            // =================================
            // COLOR
            // =================================
            float t =
                ((float)i / particleCount)
                + layer * 0.2f;

            if (t > 1.0f)
                t -= 1.0f;

            float r, g, b;

            VisualTheme_GetColor(
                s_spectrumTheme,
                t,
                &r,
                &g,
                &b);

            // ---------------------------------
            // glow
            // ---------------------------------
            float glow =
                0.6f +
                0.4f *
                sinf(time * 2.5f + i);

            glow *=
                (0.7f + energy * 0.8f);

            r *= glow;
            g *= glow;
            b *= glow;

            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            uint8_t rr =
                s_gammaTable[(uint8_t)r];

            uint8_t gg =
                s_gammaTable[(uint8_t)g];

            uint8_t bb =
                s_gammaTable[(uint8_t)b];

            // =================================
            // CORE PIXEL
            // =================================
            s_frame[y][x][0] = rr;
            s_frame[y][x][1] = gg;
            s_frame[y][x][2] = bb;

            // =================================
            // BLOOM
            // =================================
            if (x + 1 < MATRIX_WIDTH)
            {
                s_frame[y][x + 1][0] = rr / 4;
                s_frame[y][x + 1][1] = gg / 4;
                s_frame[y][x + 1][2] = bb / 4;
            }

            if (y + 1 < MATRIX_HEIGHT)
            {
                s_frame[y + 1][x][0] = rr / 4;
                s_frame[y + 1][x][1] = gg / 4;
                s_frame[y + 1][x][2] = bb / 4;
            }
        }
    }
}

void VisualModes_DrawHexGrid(const float *trail, float time)
{
    memset(s_frame, 0, sizeof(s_frame));

    // =========================================
    // AUDIO ENERGY
    // =========================================
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
        energy += trail[i];

    energy /= MATRIX_WIDTH;
    energy /= MATRIX_HEIGHT;

    // =========================================
    // GRID
    // =========================================
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            // ---------------------------------
            // staggered hex coord
            // ---------------------------------
            float xx =
                x + ((y & 1) ? 0.5f : 0.0f);

            float yy = y * 0.85f;

            // ---------------------------------
            // wave propagation
            // ---------------------------------
            float d1 =
                sqrtf(
                    (xx - 4.0f) * (xx - 4.0f) +
                    (yy - 5.0f) * (yy - 5.0f));

            float d2 =
                sqrtf(
                    (xx - 12.0f) * (xx - 12.0f) +
                    (yy - 10.0f) * (yy - 10.0f));

            float wave =
                sinf(d1 * 1.4f - time * 3.0f) +
                sinf(d2 * 1.2f - time * 2.2f);

            wave *= 0.5f;

            // ---------------------------------
            // normalize
            // ---------------------------------
            float t =
                0.5f + 0.5f * wave;

            // audio reactive
            t *=
                0.45f +
                energy * 1.4f;

            if (t > 1.0f)
                t = 1.0f;

            // =================================
            // COLOR
            // =================================
            float r, g, b;

            VisualTheme_GetColor(
                s_spectrumTheme,
                t,
                &r,
                &g,
                &b);

            // =================================
            // CELL PULSE
            // =================================
            float pulse =
                0.75f +
                0.25f *
                sinf(time * 4.0f + x + y);

            r *= pulse;
            g *= pulse;
            b *= pulse;

            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            s_frame[y][x][0] =
                s_gammaTable[(uint8_t)r];

            s_frame[y][x][1] =
                s_gammaTable[(uint8_t)g];

            s_frame[y][x][2] =
                s_gammaTable[(uint8_t)b];
        }
    }
}

void VisualModes_DrawLaserScan(const float *trail, float time)
{
    static float decay[MATRIX_HEIGHT][MATRIX_WIDTH];

    // =========================================
    // AUDIO ENERGY
    // =========================================
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
        energy += trail[i];

    energy /= MATRIX_WIDTH;
    energy /= MATRIX_HEIGHT;

    // =========================================
    // FADE OLD FRAME
    // =========================================
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            decay[y][x] *= 0.90f;

            if (decay[y][x] < 0.01f)
                decay[y][x] = 0.0f;
        }
    }

    // =========================================
    // MULTI LASER
    // =========================================
    for (int beam = 0; beam < 4; beam++)
    {
        float speed =
            1.0f + beam * 0.45f;

        float angle =
            time * speed +
            beam * 1.57f;

        float sx =
            MATRIX_WIDTH * 0.5f;

        float sy =
            MATRIX_HEIGHT * 0.5f;

        float dx = cosf(angle);
        float dy = sinf(angle);

        // beam length
        for (float t = 0; t < 12.0f; t += 0.25f)
        {
            int x =
                (int)(sx + dx * t);

            int y =
                (int)(sy + dy * t);

            if (x < 0 || x >= MATRIX_WIDTH)
                continue;

            if (y < 0 || y >= MATRIX_HEIGHT)
                continue;

            // stronger center
            float power =
                1.0f - (t / 12.0f);

            power *=
                0.6f + energy * 1.2f;

            if (power > decay[y][x])
                decay[y][x] = power;
        }
    }

    // =========================================
    // DRAW
    // =========================================
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            float t = decay[y][x];

            float r, g, b;

            VisualTheme_GetColor(
                s_spectrumTheme,
                t,
                &r,
                &g,
                &b);

            // laser sharpness
            r *= 1.2f;
            g *= 1.2f;
            b *= 1.2f;

            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            s_frame[y][x][0] =
                s_gammaTable[(uint8_t)r];

            s_frame[y][x][1] =
                s_gammaTable[(uint8_t)g];

            s_frame[y][x][2] =
                s_gammaTable[(uint8_t)b];
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
// MULTI SHOCKWAVE MODE
// Product Grade Ripple Engine
// ======================================================

typedef struct
{
    float radius;
    float power;
    uint8_t active;

} Shockwave_t;

#define MAX_SHOCKWAVES  6

void VisualModes_DrawShockwave(const float *trail, float time)
{
    static Shockwave_t waves[MAX_SHOCKWAVES];

    memset(s_frame, 0, sizeof(s_frame));

    // ==================================================
    // AUDIO ENERGY
    // ==================================================
    float energy = 0.0f;

    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        energy += trail[i];
    }

    energy /= MATRIX_WIDTH;
    energy /= MATRIX_HEIGHT;

    // ==================================================
    // BEAT DETECT
    // ==================================================
    static float beatCooldown = 0.0f;

    beatCooldown -= 0.016f;

    if (beatCooldown < 0.0f)
        beatCooldown = 0.0f;

    // 일정 energy 넘으면 새 shockwave 생성
    if (energy > 0.18f && beatCooldown <= 0.0f)
    {
        for (int i = 0; i < MAX_SHOCKWAVES; i++)
        {
            if (!waves[i].active)
            {
                waves[i].active = 1;

                waves[i].radius = 0.0f;

                waves[i].power =
                    1.2f +
                    energy * 2.5f;

                beatCooldown = 0.18f;

                break;
            }
        }
    }

    // ==================================================
    // CENTER
    // ==================================================
    float cx = MATRIX_WIDTH  * 0.5f;
    float cy = MATRIX_HEIGHT * 0.5f;

    // ==================================================
    // UPDATE WAVES
    // ==================================================
    for (int i = 0; i < MAX_SHOCKWAVES; i++)
    {
        if (!waves[i].active)
            continue;

        // expand
        waves[i].radius +=
            0.30f +
            energy * 0.7f;

        // decay
        waves[i].power *= 0.985f;

        // deactivate
        if (waves[i].radius > 20.0f ||
            waves[i].power  < 0.05f)
        {
            waves[i].active = 0;
        }
    }

    // ==================================================
    // DRAW FIELD
    // ==================================================
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            float dx = x - cx;
            float dy = y - cy;

            float dist =
                sqrtf(dx * dx + dy * dy);

            float brightness = 0.0f;

            // ==========================================
            // ACCUMULATE WAVES
            // ==========================================
            for (int i = 0; i < MAX_SHOCKWAVES; i++)
            {
                if (!waves[i].active)
                    continue;

                float d =
                    fabsf(
                        dist -
                        waves[i].radius);

                // thick wave
                float ring =
                    1.0f -
                    (d / 3.2f);

                if (ring < 0.0f)
                    ring = 0.0f;

                ring *= ring;

                brightness +=
                    ring *
                    waves[i].power;
            }

            // ==========================================
            // CENTER GLOW
            // ==========================================
            float center =
                1.0f -
                (dist / 8.0f);

            if (center < 0.0f)
                center = 0.0f;

            brightness +=
                center * 0.06f;

            // clamp
            if (brightness > 2.0f)
                brightness = 2.0f;

            // ==========================================
            // COLOR FLOW
            // ==========================================
            float colorT =
                fmodf(
                    dist * 0.10f +
                    time * 0.02f,
                    1.0f);

            float r, g, b;

            VisualTheme_GetColor(
                s_spectrumTheme,
                colorT,
                &r,
                &g,
                &b);

            // ==========================================
            // APPLY
            // ==========================================
            r *= brightness;
            g *= brightness;
            b *= brightness;

            // wave highlight
            if (brightness > 1.0f)
            {
                r *= 1.15f;
                g *= 1.15f;
                b *= 1.15f;
            }

            // clamp
            if (r > 255.0f) r = 255.0f;
            if (g > 255.0f) g = 255.0f;
            if (b > 255.0f) b = 255.0f;

            uint8_t rr =
                s_gammaTable[(uint8_t)r];

            uint8_t gg =
                s_gammaTable[(uint8_t)g];

            uint8_t bb =
                s_gammaTable[(uint8_t)b];

            s_frame[y][x][0] = rr;
            s_frame[y][x][1] = gg;
            s_frame[y][x][2] = bb;

            // ==========================================
            // BLOOM
            // ==========================================
            if (brightness > 0.7f)
            {
                if (x + 1 < MATRIX_WIDTH)
                {
                    s_frame[y][x + 1][0] = rr / 4;
                    s_frame[y][x + 1][1] = gg / 4;
                    s_frame[y][x + 1][2] = bb / 4;
                }

                if (y + 1 < MATRIX_HEIGHT)
                {
                    s_frame[y + 1][x][0] = rr / 4;
                    s_frame[y + 1][x][1] = gg / 4;
                    s_frame[y + 1][x][2] = bb / 4;
                }
            }
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
