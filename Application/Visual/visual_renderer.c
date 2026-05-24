/*
 * visual_renderer.c
 *
 *  Created on: 2026. 5. 18.
 *      Author: nugur
 */

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
static VisualMode_t s_visualMode = VISUAL_MODE_RAINBOW;

// ======================================================
// SPECTRUM + MIRROR 공유 테마
// ======================================================
uint8_t s_spectrumTheme = 0;

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

	if (s_spectrumTheme >= 6)
	{
		s_spectrumTheme = 0;
	}
}

// ======================================================
// DRAW
// ======================================================
void VisualRenderer_Draw(const float *trail, const float *peakHold)
{
	//s_modeTick++;

	// ============================================== ====
	// AUTO MODE CHANGE
	// ==================================================
	if (s_visualMode == VISUAL_MODE_RAINBOW)
	{
		static uint32_t mirrorTick = 0;

		mirrorTick++;

		if (mirrorTick >= 700)
		{
			mirrorTick = 0;
			VisualRenderer_NextSpectrumTheme();
		}
	}

	/*if (s_modeTick > 500)
	{
		s_modeTick = 0;

		// spectrum 모드, mirror 모드일 때만
		// 내부 테마 자동 순환

		if (s_visualMode == VISUAL_MODE_SPECTRUM ||
			s_visualMode == VISUAL_MODE_MIRROR)
		{
			VisualRenderer_NextSpectrumTheme();
		}
	}*/


	// ==================================================
	// DRAW MODE
	// ==================================================
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
			VisualModes_DrawMirror_Full(trail,peakHold);
			break;

		// ==============================================
		// MIRROR_CENTER
		// ==============================================
		case VISUAL_MODE_MIRROR_CENTER:
			VisualModes_DrawMirror_Center(trail,peakHold);
			break;

		// ==============================================
		// WATERFALL
		// ==============================================
		case VISUAL_MODE_WATERFALL:
			VisualModes_DrawWaterfall(trail,peakHold);
			break;

		// ==============================================
		// RAINBOW
		// ==============================================
		case VISUAL_MODE_RAINBOW:
			VisualModes_DrawRainbow(trail);
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
