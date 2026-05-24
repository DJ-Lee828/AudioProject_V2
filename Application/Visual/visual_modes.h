/*
 * visual_modes.h
 *
 *  Created on: 2026. 5. 21.
 *      Author: nugur
 */

#ifndef PERIPH_TIM_VISUAL_MODES_H_
#define PERIPH_TIM_VISUAL_MODES_H_

#include <stdint.h>
#include "visual_renderer.h"

#define MATRIX_WIDTH 16U
#define MATRIX_HEIGHT 16U
// 기존에 0x35였음
#define LED_ON_R  0x35
#define LED_ON_G  0x35
#define LED_ON_B  0x35

// ======================================================
// INIT
// ======================================================
void VisualModes_Init(void);
void VisualModes_Clear(void);

// ======================================================
// SPECTRUM THEMES
// ======================================================
void VisualModes_DrawSpectrum1(const float *trail,const float *peakHold);
void VisualModes_DrawSpectrum2(const float *trail,const float *peakHold);
void VisualModes_DrawSpectrum3(const float *trail,const float *peakHold);
void VisualModes_DrawSpectrum4(const float *trail,const float *peakHold);
void VisualModes_DrawSpectrum5(const float *trail,const float *peakHold);
void VisualModes_DrawSpectrum6(const float *trail,const float *peakHold);

// ======================================================
// MODES
// ======================================================
void VisualModes_DrawMirror_Full(const float *trail,const float *peakHold);
void VisualModes_DrawMirror_Center(const float *trail,const float *peakHold);
void VisualModes_DrawWaterfall(const float *trail,const float *peakHold);
void VisualModes_DrawRainbow(const float *trail);

// ======================================================
// GET FRAME
// ======================================================
const uint8_t *VisualModes_GetFrame(void);

#endif /* PERIPH_TIM_VISUAL_MODES_H_ */
