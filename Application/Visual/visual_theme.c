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
// THEME COLOR
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
	// THEME 1
	// PINK -> WHITE -> CYAN
	// ==================================================
	case 0:

		if (t < 0.5f)
		{
			float k = t / 0.5f;

			*r = LED_ON_R;
			*g = k * (LED_ON_G * 0.7f);
			*b = LED_ON_B;
		}
		else
		{
			float k =
				(t - 0.5f) / 0.5f;

			*r =
				(1.0f - k) * LED_ON_R;

			*g =
				LED_ON_G;

			*b =
				LED_ON_B;
		}

		break;

	// ==================================================
	// THEME 2
	// CYAN -> WHITE -> GREEN
	// ==================================================
	case 1:

		if (t < 0.5f)
		{
			float k = t / 0.5f;

			*r = k * (LED_ON_R * 0.8f);
			*g = k * LED_ON_G;
			*b = LED_ON_B;
		}
		else
		{
			float k =
				(t - 0.5f) / 0.5f;

			*r =
				(1.0f - k) * (LED_ON_R * 0.8f);

			*g =
				LED_ON_G;

			*b =
				(1.0f - k) * LED_ON_B;
		}

		break;

	// ==================================================
	// THEME 3
	// GREEN -> WHITE -> PINK -> BLUE
	// ==================================================

	case 2:

		if (t < 0.33f)
		{
			float k =
				t / 0.33f;

			*r =
				k * (LED_ON_R * 0.8f);

			*g =
				LED_ON_G;

			*b =
				k * LED_ON_B;
		}
		else if (t < 0.66f)
		{
			float k =
				(t - 0.33f) / 0.33f;

			*r =
				LED_ON_R;

			*g =
				(1.0f - k) * (LED_ON_G * 0.8f);

			*b =
				LED_ON_B;
		}
		else
		{
			float k =
				(t - 0.66f) / 0.34f;

			*r =
				(1.0f - k) * LED_ON_R;

			*g =
				0;

			*b =
				LED_ON_B;
		}

		break;

	// ==================================================
	// THEME 4
	// GREEN -> YELLOW -> PINK -> BLUE -> WHITE
	// ==================================================

	case 3:

		if (t < 0.25f)
		{
			float k =
				t / 0.25f;

			*r =
				k * (LED_ON_R * 0.9f);

			*g =
				LED_ON_G;

			*b =
				0;
		}
		else if (t < 0.50f)
		{
			float k =
				(t - 0.25f) / 0.25f;

			*r =
				LED_ON_R;

			*g =
				(1.0f - k) * LED_ON_G;

			*b =
				k * LED_ON_B;
		}
		else if (t < 0.75f)
		{
			float k =
				(t - 0.50f) / 0.25f;

			*r =
				(1.0f - k) * LED_ON_R;

			*g =
				0;

			*b =
				LED_ON_B;
		}
		else
		{
			float k =
				(t - 0.75f) / 0.25f;

			*r =
				k * (LED_ON_R * 0.8f);

			*g =
				k * (LED_ON_G * 0.8f);

			*b =
				LED_ON_B;
		}

		break;

	// ==================================================
	// THEME 5
	// RED -> ORANGE -> YELLOW -> WHITE
	// ==================================================

	case 4:

		if (t < 0.33f)
		{
			float k =
				t / 0.33f;

			*r =
				LED_ON_R;

			*g =
				k * (LED_ON_G * 0.35f);

			*b =
				0;
		}
		else if (t < 0.66f)
		{
			float k =
				(t - 0.33f) / 0.33f;

			*r =
				LED_ON_R;

			*g =
				(0.35f + 0.55f * k)
				* LED_ON_G;

			*b =
				0;
		}
		else
		{
			float k =
				(t - 0.66f) / 0.34f;

			*r =
				LED_ON_R;

			*g =
				LED_ON_G;

			*b =
				k * (LED_ON_B * 0.7f);
		}

		break;

	// ==================================================
	// THEME 6
	// NEON PURPLE → DEEP BLUE → ELECTRIC VIOLET
	// ==================================================

	default:

		if (t < 0.4f)
			{
				float k = t / 0.4f;

				*r = k * LED_ON_R * 0.6f;
				*g = 0;
				*b = LED_ON_B;
			}
			else if (t < 0.7f)
			{
				float k = (t - 0.4f) / 0.3f;

				*r = LED_ON_R * 0.7f;
				*g = 0;
				*b = (1.0f - k) * LED_ON_B;
			}
			else
			{
				float k = (t - 0.7f) / 0.3f;

				*r = (1.0f - k) * LED_ON_R * 0.5f;
				*g = k * 20.0f;
				*b = LED_ON_B;
			}

			break;
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
		case 0:

			// neon green

			*r = 8;
			*g = 30;
			*b = 8;

			break;

		case 1:

			// purple pink

			*r = 35;
			*g = 10;
			*b = 55;

			break;

		case 2:

			// cyan blue

			*r = 0;
			*g = 28;
			*b = 45;

			break;

		case 3:

			// soft white

			*r = 35;
			*g = 35;
			*b = 35;

			break;

		case 4:

			// ice blue

			*r = 12;
			*g = 32;
			*b = 50;

			break;

		default:

			// neon purple

			*r = 55;
			*g = 0;
			*b = 77;

			break;
	}
}
