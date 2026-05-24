/*
 * visual_theme.h
 *
 *  Created on: 2026. 5. 22.
 *      Author: nugur
 */

#ifndef PERIPH_TIM_VISUAL_THEME_H_
#define PERIPH_TIM_VISUAL_THEME_H_

#include <stdint.h>

// ======================================================
// THEME COUNT
// ======================================================
#define VISUAL_THEME_COUNT 6

// ======================================================
// COLOR STRUCT
// ======================================================
typedef struct
{
	float r;
	float g;
	float b;

} Color_t;

// ======================================================
// API
// ======================================================
void VisualTheme_GetColor(
	uint8_t theme,
	float t,
	float *r,
	float *g,
	float *b);

void VisualTheme_GetPeakColor(
	uint8_t theme,
	float *r,
	float *g,
	float *b);

#endif /* PERIPH_TIM_VISUAL_THEME_H_ */
