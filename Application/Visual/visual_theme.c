/*
	* visual_theme.c
	*
	*  Created on: 2026. 5. 22.
	*      Author: nugur
	*/

#include "visual_theme.h"
#include "visual_renderer.h"

#include <math.h>

// ======================================================
// helper
// ======================================================
static inline float clampf(float x, float max)
{
    return (x > max) ? max : x;
}

// smoothstep
static inline float smoothstep(float x)
{
    return x * x * (3.0f - 2.0f * x);
}

// ======================================================
// COLOR THEME
// ======================================================
void VisualTheme_GetColor(
    uint8_t theme,
    float t,
    float *r,
    float *g,
    float *b)
{
    switch (theme)
    {
    // ==================================================
    // CYBER AURORA
    // ==================================================
    case 0:
    {
        if (t < 0.25f)
        {
            float k = smoothstep(t / 0.25f);

            *r = LED_ON_R * (1.00f - 0.65f * k);
            *g = LED_ON_G * (0.18f + 0.82f * k);
            *b = LED_ON_B * 1.45f;
        }
        else if (t < 0.55f)
        {
            float k = smoothstep((t - 0.25f) / 0.30f);

            *r = LED_ON_R * (0.35f - 0.22f * k);
            *g = LED_ON_G * 1.30f;
            *b = LED_ON_B * (1.45f - 1.10f * k);
        }
        else
        {
            float k = smoothstep((t - 0.55f) / 0.45f);

            *r = LED_ON_R * (0.13f + 1.35f * k);
            *g = LED_ON_G * 1.25f;
            *b = LED_ON_B * (0.35f - 0.35f * k);
        }
        break;
    }

    // ==================================================
    // ICE BLUE
    // ==================================================
    case 1:
    {
        if (t < 0.30f)
        {
            float k = smoothstep(t / 0.30f);

            *r = LED_ON_R * (0.25f + 0.75f * k);
            *g = LED_ON_G * 0.03f;
            *b = LED_ON_B * 1.45f;
        }
        else if (t < 0.65f)
        {
            float k = smoothstep((t - 0.30f) / 0.35f);

            *r = LED_ON_R * 1.15f;
            *g = LED_ON_G * (0.03f + 0.15f * k);
            *b = LED_ON_B * (1.45f - 0.30f * k);
        }
        else
        {
            float k = smoothstep((t - 0.65f) / 0.35f);

            *r = LED_ON_R * (1.15f - 0.45f * k);
            *g = LED_ON_G * (0.18f - 0.08f * k);
            *b = LED_ON_B * (1.15f + 0.25f * k);
        }
        break;
    }

    // ==================================================
    // NEON MAGENTA
    // ==================================================
    case 2:
    {
        if (t < 0.35f)
        {
            float k = smoothstep(t / 0.35f);

            *r = LED_ON_R * 0.02f;
            *g = LED_ON_G * (0.05f + 0.28f * k);
            *b = LED_ON_B * (1.15f + 0.40f * k);
        }
        else if (t < 0.70f)
        {
            float k = smoothstep((t - 0.35f) / 0.35f);

            *r = LED_ON_R * (0.02f + 0.06f * k);
            *g = LED_ON_G * (0.33f + 0.85f * k);
            *b = LED_ON_B * 1.55f;
        }
        else
        {
            float k = smoothstep((t - 0.70f) / 0.30f);

            *r = LED_ON_R * (0.08f + 0.18f * k);
            *g = LED_ON_G * (1.18f - 0.10f * k);
            *b = LED_ON_B * (1.55f - 0.05f * k);
        }
        break;
    }

    // ==================================================
    // RGB LASER
    // ==================================================
    case 3:
    {
        if (t < 0.25f)
        {
            float k = smoothstep(t / 0.25f);

            *r = LED_ON_R * (0.05f + 1.00f * k);
            *g = LED_ON_G * 1.20f;
            *b = 0;
        }
        else if (t < 0.50f)
        {
            float k = smoothstep((t - 0.25f) / 0.25f);

            *r = LED_ON_R * 1.20f;
            *g = LED_ON_G * (1.20f - 1.05f * k);
            *b = LED_ON_B * (1.20f * k);
        }
        else if (t < 0.75f)
        {
            float k = smoothstep((t - 0.50f) / 0.25f);

            *r = LED_ON_R * (1.20f - 1.05f * k);
            *g = LED_ON_G * (0.15f - 0.10f * k);
            *b = LED_ON_B * 1.30f;
        }
        else
        {
            float k = smoothstep((t - 0.75f) / 0.25f);

            *r = LED_ON_R * (0.15f + 0.45f * k);
            *g = LED_ON_G * (0.15f + 0.75f * k);
            *b = LED_ON_B * 1.30f;
        }
        break;
    }

    // ==================================================
    // FIRE AMBER
    // ==================================================
    case 4:
    {
        if (t < 0.30f)
        {
            float k = smoothstep(t / 0.30f);

            *r = LED_ON_R * 1.30f;
            *g = LED_ON_G * (0.03f + 0.45f * k);
            *b = 0;
        }
        else if (t < 0.65f)
        {
            float k = smoothstep((t - 0.30f) / 0.35f);

            *r = LED_ON_R * (1.30f - 0.12f * k);
            *g = LED_ON_G * (0.48f + 0.72f * k);
            *b = LED_ON_B * 0.08f;
        }
        else
        {
            float k = smoothstep((t - 0.65f) / 0.35f);

            *r = LED_ON_R * (1.18f - 0.08f * k);
            *g = LED_ON_G * 1.20f;
            *b = LED_ON_B * (0.08f + 0.40f * k);
        }
        break;
    }

    // ==================================================
    // TOXIC MINT (default)
    // ==================================================
    default:
    {
        if (t < 0.35f)
        {
            float k = smoothstep(t / 0.35f);

            *r = LED_ON_R * (0.03f - 0.02f * k);
            *g = LED_ON_G * (0.40f + 0.50f * k);
            *b = LED_ON_B * (0.22f + 0.55f * k);
        }
        else if (t < 0.70f)
        {
            float k = smoothstep((t - 0.35f) / 0.35f);

            *r = LED_ON_R * (0.01f + 0.08f * k);
            *g = LED_ON_G * (0.90f + 0.18f * k);
            *b = LED_ON_B * (0.77f - 0.20f * k);
        }
        else
        {
            float k = smoothstep((t - 0.70f) / 0.30f);

            *r = LED_ON_R * (0.09f + 0.20f * k);
            *g = LED_ON_G * 1.08f;
            *b = LED_ON_B * (0.57f + 0.23f * k);
        }
        break;
    }
    }
}

// ======================================================
// PEAK COLOR
// ======================================================
void VisualTheme_GetPeakColor(
    uint8_t theme,
    float *r,
    float *g,
    float *b)
{
    switch (theme)
    {
    case 0: *r = LED_ON_R * 0.10f; *g = LED_ON_G * 1.25f; *b = LED_ON_B * 0.12f; break;
    case 1: *r = LED_ON_R * 1.35f; *g = LED_ON_G * 0.08f; *b = LED_ON_B * 1.15f; break;
    case 2: *r = LED_ON_R * 0.05f; *g = LED_ON_G * 0.95f; *b = LED_ON_B * 1.60f; break;
    case 3: *r = LED_ON_R * 0.55f; *g = LED_ON_G * 0.85f; *b = LED_ON_B * 1.35f; break;
    case 4: *r = LED_ON_R * 1.35f; *g = LED_ON_G * 0.85f; *b = LED_ON_B * 0.05f; break;
    default:*r = LED_ON_R * 0.25f; *g = LED_ON_G * 1.25f; *b = LED_ON_B * 0.70f; break;
    }
}

// ======================================================
// MIRROR COLOR
// ======================================================
void VisualTheme_GetMirrorColor(
    uint8_t theme,
    float t,
    float *r,
    float *g,
    float *b)
{
    t = powf(t, 0.72f);

    float k = t;

    switch (theme)
    {
    case 0: *r = LED_ON_R * (0.05f + 0.12f * k); *g = LED_ON_G * (0.25f + 0.60f * k); *b = LED_ON_B * (0.40f + 0.65f * k); break;
    case 1: *r = LED_ON_R * (0.45f + 0.50f * k); *g = LED_ON_G * (0.10f + 0.35f * k); *b = LED_ON_B * (0.35f + 0.55f * k); break;
    case 2: *r = 0.03f; *g = LED_ON_G * (0.40f + 0.65f * k); *b = LED_ON_B * (0.10f + 0.25f * k); break;
    case 3: *r = LED_ON_R * (0.55f + 0.50f * k); *g = LED_ON_G * (0.20f + 0.50f * k); *b = 0; break;
    case 4: *r = LED_ON_R * (0.70f + 0.45f * k); *g = LED_ON_G * (0.55f + 0.45f * k); *b = LED_ON_B * (0.00f + 0.10f * k); break;
    default:
        {
            float v = 0.45f + 0.85f * k;
            *r = LED_ON_R * v;
            *g = LED_ON_G * v;
            *b = LED_ON_B * v;
        }
        break;
    }

    *r = clampf(*r, LED_ON_R);
    *g = clampf(*g, LED_ON_G);
    *b = clampf(*b, LED_ON_B);
}
