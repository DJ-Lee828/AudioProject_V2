/*
 * fft.h
 *
 *  Created on: 2026. 5. 23.
 *      Author: ADJ
 */

#ifndef AUDIO_FRAME_FFT_H_
#define AUDIO_FRAME_FFT_H_

#include <stdint.h>

void FFT_Init(void);
uint8_t FFT_Run(void);
const float32_t *Visual_GetTrail(void);
const float32_t *Visual_GetPeak(void);

#endif /* AUDIO_FRAME_FFT_H_ */
