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
// MODE TICK
// ======================================================
//static uint32_t s_modeTick = 0;

// ======================================================
// 현재 모드
// ======================================================
static VisualMode_t s_visualMode = VISUAL_MODE_SPECTRUM;

// ======================================================
// SPECTRUM + WATERFALL + RAINBOW 공유 테마 및 MIRROR 테마
// ======================================================
uint8_t s_spectrumTheme = 0;
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
// NEXT SPECTRUM + WATERFALL + RAINBOW THEME
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
// DRAW
// ======================================================
void VisualRenderer_Draw(const float *trail, const float *peakHold)
{
	switch (s_visualMode)
	{
		// ==============================================
		// SPECTRUM
		// ==============================================
		case VISUAL_MODE_SPECTRUM:
			switch (s_spectrumTheme)
			{
				// --------------------------------------
				// SPECTRUM 1
				// --------------------------------------
				case 0:
					VisualModes_DrawSpectrum1(trail,peakHold);
					break;

				// --------------------------------------
				// SPECTRUM 2
				// --------------------------------------
				case 1:
					VisualModes_DrawSpectrum2(trail,peakHold);
					break;
				// --------------------------------------
				// SPECTRUM 3
				// --------------------------------------
				case 2:
					VisualModes_DrawSpectrum3(trail,peakHold);
					break;

				// --------------------------------------
				// SPECTRUM 4
				// --------------------------------------
				case 3:
					VisualModes_DrawSpectrum4(trail,peakHold);
					break;

				// --------------------------------------
				// SPECTRUM 5
				// --------------------------------------
				case 4:
					VisualModes_DrawSpectrum5(trail,peakHold);
					break;

				// --------------------------------------
				// SPECTRUM 6
				// --------------------------------------
				case 5:
					VisualModes_DrawSpectrum6(trail,peakHold);
					break;

				// --------------------------------------
				// DEFAULT
				// --------------------------------------
				default:
					VisualModes_DrawSpectrum1(trail,peakHold);
					break;
			}
			break;

		// ==============================================
		// MIRROR_FULL
		// ==============================================
		case VISUAL_MODE_MIRROR_FULL:
			VisualModes_DrawMirror_Full(trail);
			break;

		// ==============================================
		// MIRROR_CENTER
		// ==============================================
		case VISUAL_MODE_MIRROR_CENTER:
			VisualModes_DrawMirror_Center(trail);
			break;

		// ==============================================
		// RAINBOW
		// ==============================================
		case VISUAL_MODE_RAINBOW:
			VisualModes_DrawRainbow(trail);
			break;

		// ==============================================
		// WATERFALL
		// ==============================================
		case VISUAL_MODE_WATERUP:
			VisualModes_DrawWaterup(trail);
			break;

		// ==============================================
		// DEFAULT
		// ==============================================
		default:
			VisualModes_DrawSpectrum1(trail, peakHold);
			break;
	}
}

const uint8_t *VisualRenderer_GetFrame(void)
{
	return VisualModes_GetFrame();
}
