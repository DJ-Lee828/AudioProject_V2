/*
 * visual_renderer.h
 *
 *  Created on: 2026. 5. 18.
 *      Author: nugur
 */

#ifndef PERIPH_TIM_VISUAL_RENDERER_H_
#define PERIPH_TIM_VISUAL_RENDERER_H_

#include <stdint.h>
#include "visual_modes.h"

// ======================================================
// VISUAL MODE
// ======================================================

typedef enum
{
	VISUAL_MODE_SPECTRUM = 0,
	VISUAL_MODE_MIRROR_FULL,
	VISUAL_MODE_MIRROR_CENTER,
	VISUAL_MODE_WATERFALL,
	VISUAL_MODE_RAINBOW,

	VISUAL_MODE_COUNT
} VisualMode_t;

// ======================================================
// API
// ======================================================
extern uint8_t s_spectrumTheme;

void VisualRenderer_Init(void);

void VisualRenderer_Clear(void);

void VisualRenderer_Draw(const float *trail,const float *peakHold);

void VisualRenderer_NextMode(void);

void VisualRenderer_NextSpectrumTheme(void);

const uint8_t *VisualRenderer_GetFrame(void);

#endif /* PERIPH_TIM_VISUAL_RENDERER_H_ */
