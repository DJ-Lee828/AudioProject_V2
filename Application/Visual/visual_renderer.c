/*
 * visual_renderer.c
 *
 *  Created on: 2026. 5. 18.
 *      Author: nugur
 */

#include "visual_theme.h"
#include "visual_renderer.h"

#include <string.h>
#include <math.h>

// ======================================================
// CURRENT VISUAL MODE
// ======================================================
static VisualMode_t s_visualMode = VISUAL_MODE_SPECTRUM;

// ======================================================
// GLOBAL TIME
// ======================================================
// animated mode 공유 time
static float s_time = 0.0f;

// ======================================================
// THEME
// ======================================================
// spectrum/rainbow/waterfall 계열 공유
uint8_t s_spectrumTheme = 0;

// mirror 전용
uint8_t s_mirrorTheme = 0;

// ======================================================
// INIT
// ======================================================
void VisualRenderer_Init(void)
{
    VisualModes_Init();
}

// ======================================================
// CLEAR
// ======================================================
void VisualRenderer_Clear(void)
{
    VisualModes_Clear();
}

// ======================================================
// NEXT MODE
// ======================================================
void VisualRenderer_NextMode(void)
{
    s_visualMode++;

    if (s_visualMode >= VISUAL_MODE_COUNT)
    {
        s_visualMode = 0;
    }
}

// ======================================================
// NEXT SPECTRUM THEME
// ======================================================
void VisualRenderer_NextSpectrumTheme(void)
{
    s_spectrumTheme++;

    if (s_spectrumTheme >= SPECTRUM_THEME_COUNT)
    {
        s_spectrumTheme = 0;
    }
}

// ======================================================
// NEXT MIRROR THEME
// ======================================================
void VisualRenderer_NextMirrorTheme(void)
{
    s_mirrorTheme++;

    if (s_mirrorTheme >= MIRROR_THEME_COUNT)
    {
        s_mirrorTheme = 0;
    }
}

// ======================================================
// UPDATE GLOBAL TIME
// ======================================================
static inline void UpdateTime(float speed)
{
    s_time += speed;

    if (s_time > 1000.0f)
    {
        s_time = 0.0f;
    }
}

// ======================================================
// DRAW
// ======================================================
void VisualRenderer_Draw(const float *trail, const float *peakHold)
{
    switch (s_visualMode)
    {
        // ==================================================
        // 1. CLASSIC ANALYZER
        // ==================================================

        // --------------------------------------------------
        // STANDARD SPECTRUM
        // --------------------------------------------------
        case VISUAL_MODE_SPECTRUM:

            switch (s_spectrumTheme)
            {
                case 0:
                    VisualModes_DrawSpectrum1(trail, peakHold);
                    break;

                case 1:
                    VisualModes_DrawSpectrum2(trail, peakHold);
                    break;

                case 2:
                    VisualModes_DrawSpectrum3(trail, peakHold);
                    break;

                case 3:
                    VisualModes_DrawSpectrum4(trail, peakHold);
                    break;

                case 4:
                    VisualModes_DrawSpectrum5(trail, peakHold);
                    break;

                case 5:
                    VisualModes_DrawSpectrum6(trail, peakHold);
                    break;

                default:
                    VisualModes_DrawSpectrum1(trail, peakHold);
                    break;
            }

            break;

        // --------------------------------------------------
        // FULL MIRROR
        // --------------------------------------------------
        case VISUAL_MODE_MIRROR_FULL:

            VisualModes_DrawMirror_Full(trail);

            break;

        // --------------------------------------------------
        // CENTER MIRROR
        // --------------------------------------------------
        case VISUAL_MODE_MIRROR_CENTER:

            VisualModes_DrawMirror_Center(trail);

            break;

        // --------------------------------------------------
        // RAINBOW ANALYZER
        // --------------------------------------------------
        case VISUAL_MODE_RAINBOW:

            VisualModes_DrawRainbow(trail);

            break;

        // --------------------------------------------------
        // PULSE ANALYZER
        // --------------------------------------------------
        case VISUAL_MODE_PULSE:

            VisualModes_DrawPulse(trail);

            break;

        // ==================================================
        // 2. REACTIVE / ENERGY MODES
        // ==================================================

        // --------------------------------------------------
        // SPARK NOISE
        // 전기 스파크 + 노이즈 파동
        // --------------------------------------------------
        case VISUAL_MODE_SPARK_NOISE:

            UpdateTime(0.016f);

            VisualModes_DrawSparkNoise(trail, s_time);

            break;

        // --------------------------------------------------
        // GLITCH GRID
        // 디지털 글리치 패턴
        // --------------------------------------------------
        case VISUAL_MODE_GLITCH_GRID:

            UpdateTime(0.016f);

            VisualModes_DrawGlitchGrid(trail, s_time);

            break;

        // --------------------------------------------------
        // GRID BREATH
        // 숨쉬는 사이버 격자
        // --------------------------------------------------
        case VISUAL_MODE_GRID_BREATH:

            UpdateTime(0.016f);

            VisualModes_DrawGridBreath(trail, s_time);

            break;

        // ==================================================
        // 3. SPACE / MOTION MODES
        // ==================================================

        // --------------------------------------------------
        // ROTATING FIELD
        // 회전하는 에너지 필드
        // --------------------------------------------------
        case VISUAL_MODE_ROTATING_FIELD:

            UpdateTime(0.016f);

            VisualModes_DrawRotatingField(trail, s_time);

            break;

        // --------------------------------------------------
        // PLASMA MODE
        // 유체 느낌 플라즈마
        // --------------------------------------------------
        case VISUAL_MODE_PLASMA_MODE:

            UpdateTime(0.016f);

            VisualModes_DrawPlasma(trail, s_time);

            break;

        // --------------------------------------------------
        // MULTI ORBIT
        // 다중 궤도 시스템
        // --------------------------------------------------
        case VISUAL_MODE_MULTI_ORBIT:

            UpdateTime(0.020f);

            VisualModes_DrawMultiOrbit(trail, s_time);

            break;

        // --------------------------------------------------
        // HEX GRID
        // 육각 패턴 사이버 모드
        // --------------------------------------------------
        case VISUAL_MODE_HEX:

            UpdateTime(0.018f);

            VisualModes_DrawHexGrid(trail, s_time);

            break;

        // --------------------------------------------------
        // LASER SCAN
        // 레이저 스캔 느낌
        // --------------------------------------------------
        case VISUAL_MODE_LASER:

            UpdateTime(0.030f);

            VisualModes_DrawLaserScan(trail, s_time);

            break;

        // ==================================================
        // 4. FLOW / DEPTH MODES
        // ==================================================

        // --------------------------------------------------
        // WATERFALL
        // depth + blur + 3D scrolling
        // --------------------------------------------------
        case VISUAL_MODE_WATERUP:

            VisualModes_DrawWaterup(trail);

            break;

		// --------------------------------------------------
		// AURORA FLOW
		// cinematic ambient curtain motion
		// premium northern-light effect
		// --------------------------------------------------
        case VISUAL_MODE_SHOCKWAVE:

        	UpdateTime(0.020f);

        	VisualModes_DrawShockwave(trail, s_time);

        	break;

        // ==================================================
        // DEFAULT
        // ==================================================
        default:

            VisualModes_DrawSpectrum1(trail, peakHold);

            break;
    }
}

// ======================================================
// GET FRAME BUFFER
// ======================================================
const uint8_t *VisualRenderer_GetFrame(void)
{
    return VisualModes_GetFrame();
}
