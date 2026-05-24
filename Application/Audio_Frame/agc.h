/*
 * agc.h
 *
 *  Created on: 2026. 5. 24.
 *      Author: ADJ
 */

#ifndef AUDIO_FRAME_AGC_H_
#define AUDIO_FRAME_AGC_H_

#include <stdint.h>

void AGC_Run(int16_t *src, int16_t *dst, uint32_t count);
float AGC_GetGain(void);

#endif /* AUDIO_FRAME_AGC_H_ */
