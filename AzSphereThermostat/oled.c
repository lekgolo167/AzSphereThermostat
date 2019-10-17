#include "oled.h"
#include <math.h>
#include <time.h>

// Status variables of I2C bus and RT core
extern uint8_t RTCore_status;
extern uint8_t lsm6dso_status;
extern uint8_t lps22hh_status;

struct HDC1080* sensor;
static float temporary = 20.0;
/**
  * @brief  OLED initialization.
  * @param  None.
  * @retval Positive if was unsuccefully, zero if was succefully.
  */
uint8_t oled_init(struct HDC1080* ptr)
{
	sensor = ptr;
	return sd1306_init();
}

// State machine to change the OLED status
void update_oled(float c, float f, float h, float t)
{
	switch (oled_menu_state)
	{
		case 0:
		{
			oled_draw_logo();
		}
		break;
		case 1:
		{
			set_temp(c, temporary);
		}
		break;
		case 2:
		{
			update_other(c, f,h);
		}
		break;
		case 3:
		{
		}
		break;
		case 4:
		{
		}
		break;
		case 5:
		{
		}
		break;
		case 6:
		{
		}
		break;
		case 7:
		{
			
		}
		break;

		default:
		break;
	}
}

/**
  * @brief  Template to show I2C bus status
  * @param  sensor_number: Sensor number
  * @param  sensor_status: Sensor status
  * @retval None.
  */
void oled_i2c_bus_status(uint8_t sensor_number)
{

	// Strings for labels
	uint8_t str_bus_sta[]   = "I2C Bus Status:";
	uint8_t str_lsmod_sta[] = "LSM6DSO Accel.:";
	uint8_t str_lps22_sta[] = "LPS22HH Barom.:";
	uint8_t str_rtcore_sta[] = "Real Time Core:";

	switch (sensor_number)
	{
		case 0:
		{
			// Clear OLED buffer
			clear_oled_buffer();

			// Draw the title
			sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, " I2C Init", FONT_SIZE_TITLE, white_pixel);

			// Draw a label at line 1
			sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, str_bus_sta, FONT_SIZE_LINE, white_pixel);

			// I2C bus OK, if not OLED doesn't show a image
			sd1306_draw_string(sizeof(str_bus_sta) * 6, OLED_LINE_1_Y, "OK", FONT_SIZE_LINE, white_pixel);
		}
		break;
		case 1:
		{
			// Draw a label at line 2
			sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, str_lsmod_sta, FONT_SIZE_LINE, white_pixel);

			// Show LSMOD status
			if (lsm6dso_status == 0)
			{
				sd1306_draw_string(sizeof(str_lsmod_sta) * 6, OLED_LINE_2_Y, "OK", FONT_SIZE_LINE, white_pixel);
			}
			else
			{
				sd1306_draw_string(sizeof(str_lsmod_sta) * 6, OLED_LINE_2_Y, "ERROR", FONT_SIZE_LINE, white_pixel);
			}
		}
		break;
		case 2:
		{
			// Draw a label at line 3
			sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, str_lps22_sta, FONT_SIZE_LINE, white_pixel);

			// Show LPS22 status
			if (lps22hh_status == 0)
			{
				sd1306_draw_string(sizeof(str_lps22_sta) * 6, OLED_LINE_3_Y, "OK", FONT_SIZE_LINE, white_pixel);
			}
			else
			{
				sd1306_draw_string(sizeof(str_lps22_sta) * 6, OLED_LINE_3_Y, "ERROR", FONT_SIZE_LINE, white_pixel);
			}
		}
		break;
		case 3:
		{
			// If we are here I2C is working well

			// Clear OLED buffer
			clear_oled_buffer();

			// Draw the title
			sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, " I2C Init", FONT_SIZE_TITLE, white_pixel);

			// Draw a label at line 1
			sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, str_bus_sta, FONT_SIZE_LINE, white_pixel);

			// I2C bus OK, if not OLED doesn't show a image
			sd1306_draw_string(sizeof(str_bus_sta) * 6, OLED_LINE_1_Y, "OK", FONT_SIZE_LINE, white_pixel);

			// Draw a label at line 2
			sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, str_lsmod_sta, FONT_SIZE_LINE, white_pixel);

			// Show LSMOD status
			if (lsm6dso_status == 0)
			{
				sd1306_draw_string(sizeof(str_lsmod_sta) * 6, OLED_LINE_2_Y, "OK", FONT_SIZE_LINE, white_pixel);
			}
			else
			{
				sd1306_draw_string(sizeof(str_lsmod_sta) * 6, OLED_LINE_2_Y, "ERROR", FONT_SIZE_LINE, white_pixel);
			}

			// Draw a label at line 3
			sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, str_lps22_sta, FONT_SIZE_LINE, white_pixel);

			// Show LPS22 status
			if (lps22hh_status == 0)
			{
				sd1306_draw_string(sizeof(str_lps22_sta) * 6, OLED_LINE_3_Y, "OK", FONT_SIZE_LINE, white_pixel);
			}
			else
			{
				sd1306_draw_string(sizeof(str_lps22_sta) * 6, OLED_LINE_3_Y, "ERROR", FONT_SIZE_LINE, white_pixel);
			}
			
			// Draw a label at line 4
			sd1306_draw_string(OLED_LINE_4_X, OLED_LINE_4_Y, str_rtcore_sta, FONT_SIZE_LINE, white_pixel);

			// Show RTcore status
			if ( RTCore_status == 0)
			{
				sd1306_draw_string(sizeof(str_rtcore_sta) * 6, OLED_LINE_4_Y, "OK", FONT_SIZE_LINE, white_pixel);
			}
			else
			{
				sd1306_draw_string(sizeof(str_rtcore_sta) * 6, OLED_LINE_4_Y, "ERROR", FONT_SIZE_LINE, white_pixel);
			}
		}
		break;
		default:
		break;
	}

	// Send the buffer to OLED RAM
	sd1306_refresh();
}


/**
  * @brief  Template to show other variables curently not available
  * @param  x: var 1
  * @param  y: var 2
  * @param  z: var 3
  * @retval None.
  */
void update_other(float x, float y, float z)
{
	uint32_t i;
	uint8_t string_data[10];

	// Strings for labels
	uint8_t str_light[] = "HDC1080: ";
	uint8_t str_tbd1[] = "HDC1080: ";
	uint8_t str_tbd2[] = "Humidity:";

	auto lines[] = { OLED_LINE_1_Y,OLED_LINE_2_Y ,OLED_LINE_3_Y ,OLED_LINE_4_Y };
	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "   TEMP", FONT_SIZE_TITLE, white_pixel);

	// Convert x value to string
	ftoa(x, string_data, 2);

	// Draw a label at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, str_light, FONT_SIZE_LINE, white_pixel);
	// Draw the value of x
	sd1306_draw_string(sizeof(str_light) * 6, OLED_LINE_1_Y, string_data, FONT_SIZE_LINE, white_pixel);
	// Draw the units of x
	sd1306_draw_string(sizeof(str_light) * 6 + (get_str_size(string_data) + 1) * 6, OLED_LINE_1_Y, " C°", FONT_SIZE_LINE, white_pixel);

	// Convert y value to string
	ftoa(y, string_data, 2);

	// Draw a label at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, str_tbd1, FONT_SIZE_LINE, white_pixel);
	// Draw the value of y
	sd1306_draw_string(sizeof(str_tbd1) * 6, OLED_LINE_2_Y, string_data, FONT_SIZE_LINE, white_pixel);
	// Draw the units of y
	sd1306_draw_string(sizeof(str_tbd1) * 6 + (get_str_size(string_data) + 1) * 6, OLED_LINE_2_Y, " F°", FONT_SIZE_LINE, white_pixel);

	// Convert z value to string
	ftoa(z, string_data, 2);
	// Draw a label at line 3
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, str_tbd2, FONT_SIZE_LINE, white_pixel);
	// Draw the value of z
	sd1306_draw_string(sizeof(str_tbd2) * 6, OLED_LINE_3_Y, string_data, FONT_SIZE_LINE, white_pixel);
	// Draw the units of z
	sd1306_draw_string(sizeof(str_tbd2) * 6 + (get_str_size(string_data) + 1) * 6, OLED_LINE_3_Y, "%", FONT_SIZE_LINE, white_pixel);
	if(oled_scroll_counter >=0 && oled_scroll_counter <=6)
	sd1306_invert_line(0, OLED_WIDTH, lines[oled_scroll_counter/2]-1, lines[oled_scroll_counter / 2] +9);
	else {
		sd1306_invert_line(0, OLED_WIDTH, oled_scroll_counter - 6, oled_scroll_counter+4);
	}
	// Send the buffer to OLED RAM
	sd1306_refresh();
}
/**
  * @brief  Template to show other variables curently not available
  * @param  x: var 1
  * @param  y: var 2
  * @param  z: var 3
  * @retval None.
  */
void set_temp(float c, float t)
{
	uint32_t i;
	uint8_t string_data[10];

	// Strings for labels
	uint8_t str_light[] = "Room: ";
	uint8_t str_tbd1[] = "Set: ";

	auto lines[] = { OLED_LINE_1_Y,OLED_LINE_2_Y ,OLED_LINE_3_Y ,OLED_LINE_4_Y };
	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Edit Temp", FONT_SIZE_TITLE, white_pixel);

	// Convert x value to string
	ftoa(c, string_data, 1);

	// Draw a label at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, str_light, FONT_SIZE_TITLE, white_pixel);
	// Draw the value of x
	sd1306_draw_string(sizeof(str_light) * 8, OLED_LINE_1_Y, string_data, FONT_SIZE_TITLE, white_pixel);
	// Draw the units of x
	sd1306_draw_string(sizeof(str_light) * 8 + (get_str_size(string_data) + 1) * 8, OLED_LINE_1_Y, " C°", FONT_SIZE_TITLE, white_pixel);

	// Convert y value to string
	ftoa(t, string_data, 1);

	// Draw a label at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_3_Y, str_tbd1, FONT_SIZE_TITLE, white_pixel);
	// Draw the value of y
	sd1306_draw_string(sizeof(str_tbd1) * 8, OLED_LINE_3_Y, string_data, FONT_SIZE_TITLE, white_pixel);
	// Draw the units of y
	sd1306_draw_string(sizeof(str_tbd1) * 8 + (get_str_size(string_data) + 1) * 8, OLED_LINE_3_Y, " C°", FONT_SIZE_TITLE, white_pixel);

	if (!edit && oled_scroll_counter >= 0 && oled_scroll_counter <= 6)
		sd1306_invert_line(0, OLED_WIDTH, lines[oled_scroll_counter] - 1, lines[oled_scroll_counter] + 15);
	else {
		temporary++;
	}
	// Send the buffer to OLED RAM
	sd1306_refresh();
}
/**
  * @brief  Template to show a logo
  * @param  None.
  * @retval None.
  */
void oled_draw_logo(void)
{
	// Copy image_avnet to OLED buffer
	sd1306_draw_img(Image_avnet_bmp);

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

// reverses a string 'str' of length 'len' 
static void reverse(uint8_t *str, int32_t len)
{
	int32_t i = 0;
	int32_t j = len - 1;
	int32_t temp;

	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

/**
  * @brief  Converts a given integer x to string uint8_t[]
  * @param  x: x integer input
  * @param  str: uint8_t array output
  * @param  d: Number of zeros added
  * @retval i: number of digits
  */
int32_t intToStr(int32_t x, uint8_t str[], int32_t d)
{
	int32_t i = 0;
	uint8_t flag_neg = 0;

	if (x < 0)
	{
		flag_neg = 1;
		x *= -1;
	}
	while (x)
	{
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then 
	// add 0s at the beginning 
	while (i < d)
	{
		str[i++] = '0';
	}

	if (flag_neg)
	{
		str[i] = '-';
		i++;
	}

	reverse(str, i);
	str[i] = '\0';
	return i;
}

/**
  * @brief  Converts a given integer x to string uint8_t[]
  * @param  n: float number to convert
  * @param  res:
  * @param  afterpoint:
  * @retval None.
  */
void ftoa(float n, uint8_t *res, int32_t afterpoint)
{
	// Extract integer part 
	int32_t ipart = (int32_t)n;

	// Extract floating part 
	float fpart = n - (float)ipart;

	int32_t i;

	if (ipart < 0)
	{
		res[0] = '-';
		res++;
		ipart *= -1;
	}

	if (fpart < 0)
	{
		fpart *= -1;

		if (ipart == 0)
		{
			res[0] = '-';
			res++;
		}
	}

	// convert integer part to string 
	i = intToStr(ipart, res, 1);

	// check for display option after point 
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot 

		// Get the value of fraction part upto given no. 
		// of points after dot. The third parameter is needed 
		// to handle cases like 233.007 
		fpart = fpart * pow(10, afterpoint);

		intToStr((int32_t)fpart, res + i + 1, afterpoint);
	}
}

// AVNET logo

const unsigned char Image_avnet_bmp[BUFFER_SIZE] =
{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,128,240,240,240,240, 48,  0,  0,112,
  240,240,240,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,112,
  240,240,240,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,224,240,240,
  240, 16,  0,  0,  0,  0,  0,  0,  0,  0,240,240,240,240,224,128,
	0,  0,  0,  0,  0,  0,  0,  0,240,240,240,240,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,240,240,240,240,112,112,112,112,112,112,
  112,112,112,112,112,  0,  0,  0,  0,  0,  0,  0,  0,112,112,112,
  112,112,112,112,240,240,240,240,112,112,112,112,112,112,  0,  0,
	0,  0,  0,  0,  0,224,252,255,255,255, 15,  1,  0,  0,  0,  0,
	3, 15,127,255,255,248,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	7, 31,255,255,254,240,  0,  0,  0,  0,224,248,255,255,127,  7,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255, 15, 31,
  127,252,248,224,224,128,  0,  0,255,255,255,255,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,255,255,255,255,224,224,224,224,224,224,
  224,224,224,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,128,240,254,255,127, 15,  1,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  3, 31,255,255,252,224,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  7, 63,255,255,248,240,254,255,255, 31,  3,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,
	0,  1,  3, 15, 15, 63,126,252,255,255,255,255,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,255,255,255,255,129,129,129,129,129,129,
  129,129,129,129,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  7,  7,  7,  3,  0,  0,  0, 12, 14, 14, 14, 14, 14, 14,
   14, 14, 12,  0,  0,  0,  7,  7,  7,  7,  4,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  1,  7,  7,  7,  7,  1,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  1,  3,  7,  7,  7,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


uint8_t get_str_size(uint8_t * str)
{
	uint8_t legth = 0;
	while (*(str) != NULL)
	{
		str++;
		legth++;
	}
	return legth;
}