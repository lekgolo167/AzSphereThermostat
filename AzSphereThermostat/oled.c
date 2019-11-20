#include "oled.h"
#include <math.h>
#include <time.h>

// Status variables of I2C bus and RT core
extern uint8_t RTCore_status;
extern uint8_t lsm6dso_status;
extern uint8_t lps22hh_status;

int8_t line_selection[] = { -20, OLED_LINE_1_Y,OLED_LINE_2_Y ,OLED_LINE_3_Y ,OLED_LINE_4_Y };
enum menu_items { TARGET, THRESHOLDLOWER, THRESHOLDUPPER, BASELINE, TOTALSAMPLES, SAMPLEPERIOD, SCREENTIMEOUT, MOTIONDETECTION};
struct HDC1080* sensor;
struct thermostatSettings* settings;
char buffer[25];
/**
  * @brief  OLED initialization.
  * @param  None.
  * @retval Positive if unsucceful, zero if succeful.
  */
uint8_t oled_init(struct HDC1080* s_ptr, struct thermostatSettings* t_ptr)
{
	sensor = s_ptr;
	settings = t_ptr;
	return sd1306_init();
}

// Update settings
void updateUserSettings()
{
	switch (oled_menu_item)
	{
	case TARGET:
	{
		if (temporary_setting >= settings->baselineTemp_F && temporary_setting <= 95)
			settings->targetTemp_F = temporary_setting;
	}
	break;
	case THRESHOLDLOWER:
	{
		if (temporary_setting >= 0 && temporary_setting <= 5)
			settings->lower_threshold = temporary_setting;
	}
	break;
	case THRESHOLDUPPER:
	{
		if (temporary_setting >= 0 && temporary_setting <= 5)
			settings->upper_threshold = temporary_setting;
	}
	break;
	case BASELINE:
	{
		if (temporary_setting >= 45 && temporary_setting <= 60)
			settings->baselineTemp_F = temporary_setting;
	}
	break;
	case TOTALSAMPLES:
	{
		if (temporary_setting >= 1 && temporary_setting <= 20)
			settings->totalSamples = temporary_setting;
	}
	break;
	case SAMPLEPERIOD:
	{
		if (temporary_setting >= 5 && temporary_setting <= 300) {
			reconfigureTimer = true;
			struct timespec t = { temporary_setting, 0 };
			settings->samplePeriod = t;
		}
	}
	break;
	case SCREENTIMEOUT:
	{
		if (temporary_setting >= 1 && temporary_setting <= 180)
			settings->screenTimeoutSec = temporary_setting;
	}
	break;
	case MOTIONDETECTION:
	{
		if(temporary_setting >= 6 && temporary_setting <= 96)
			settings->motionDetectorSec = temporary_setting * 3600; // convert to seconds from hours
	}
	break;
	default:
		break;
	}
}

// Update settings
void updateTemporarySettingValue()
{
	switch (oled_menu_item)
	{
	case TARGET:
	{
		temporary_setting = settings->targetTemp_F;
	}
	break;
	case THRESHOLDLOWER:
	{
		temporary_setting = settings->lower_threshold;
	}
	break;
	case THRESHOLDUPPER:
	{
		temporary_setting = settings->upper_threshold;
	}
	break;
	case BASELINE:
	{
		temporary_setting = settings->baselineTemp_F;
	}
	break;
	case TOTALSAMPLES:
	{
		temporary_setting = settings->totalSamples;
	}
	break;
	case SAMPLEPERIOD:
	{
		temporary_setting = settings->samplePeriod.tv_sec;
	}
	break;
	case SCREENTIMEOUT:
	{
		temporary_setting = settings->screenTimeoutSec;
	}
	break;
	case MOTIONDETECTION:
	{
		temporary_setting = settings->motionDetectorSec/3600; // convert from seconds to hours
	}
	break;
	default:
		break;
	}
}

// State machine to change the OLED status
void update_oled()
{
	if (!oledScreenON) {
		return;
	}
	switch (oled_menu_state)
	{
		case 0:
		{
			oled_draw_logo();
		}
		break;
		case 1:
		{
			set_temp();
		}
		break;
		case 2:
		{
			showSensorData();
		}
		break;
		case 3:
		{
			set_thresh();
		}
		break;
		case 4:
		{
			sampleSettings();
		}
		break;
		case 5:
		{
			motionSettings();
		}
		break;
		case 6:
		{
			displayDateAndTime();
		}
		break;
		case 7:
		{
			displayCycle();
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

void showSensorData()
{
	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Sensor Data", FONT_SIZE_TITLE, white_pixel);

	// Convert celcius temperature to string
	sprintf(buffer, "HDC1080:  %.1f C°\0", sensor->temp_C);

	// Draw celcius temperature at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Convert fahrenheit temperature to string
	sprintf(buffer, "HDC1080:  %.1f F°\0", sensor->temp_F);

	// Draw fahrenheit temperature at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Convert humidity to string
	sprintf(buffer, "Humidity: %.1f %%\0", sensor->humidity);

	// Draw humidity at line 3
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void set_temp()
{
	int8_t items[] = { -1, TARGET };

	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Edit Temp", FONT_SIZE_TITLE, white_pixel);

	// Convert temperature to string
	sprintf(buffer, "Air: %.1fF°\0", sensor->temp_F);

	// Draw room temperature label at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_TITLE, white_pixel);

	// Convert target temperature to string
	if (edit_oled_menu && oled_menu_item == TARGET) {
		sprintf(buffer, "Set: %d.0F°\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Set: %.1fF°\0", settings->targetTemp_F);
	}

	// Draw target temperature at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_3_Y, buffer, FONT_SIZE_TITLE, white_pixel);

	// Confine the scroll line of the menu
	boundScrollCounter(3, 3, 15);

	// Save which item we are editing
	oled_menu_item = items[(oled_scroll_counter-2)];

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void set_thresh()
{
	int8_t items[] = {-1, THRESHOLDLOWER, THRESHOLDUPPER, BASELINE};

	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Settings", FONT_SIZE_TITLE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == THRESHOLDLOWER) {
		sprintf(buffer, "Thresh Lower:  %d.0 F°\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Thresh Lower:  %.1f F°\0", settings->lower_threshold);
	}
	// Draw lower threshold at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == THRESHOLDUPPER) {
		sprintf(buffer, "Thresh Upper:  %d.0 F°\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Thresh Upper:  %.1f F°\0", settings->upper_threshold);
	}
	// Draw upper threshold at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == BASELINE) {
		sprintf(buffer, "Baseline:     %d.0 F°\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Baseline:     %.1f F°\0", settings->baselineTemp_F);
	}

	// Draw baseline temperautre at line 3
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Confine the scroll line of the menu
	boundScrollCounter(1, 3, 9);

	// Save which item we are editing
	oled_menu_item = items[oled_scroll_counter];

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void sampleSettings()
{
	int8_t items[] = { -1, TOTALSAMPLES, SAMPLEPERIOD };

	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Settings", FONT_SIZE_TITLE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == TOTALSAMPLES) {
		sprintf(buffer, "Total Samples: %d\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Total Samples: %d\0", settings->totalSamples);
	}
	// Draw total samples at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == SAMPLEPERIOD) {
		sprintf(buffer, "Sample Period: %ds\0", temporary_setting);
		
	}
	else {
		sprintf(buffer, "Sample Period: %ds\0", settings->samplePeriod.tv_sec);

	}
	// Draw sample period in seconds at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Confine the scroll line of the menu
	boundScrollCounter(1, 3, 9);

	// Save which item we are editing
	oled_menu_item = items[oled_scroll_counter];

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void motionSettings()
{
	int8_t items[] = { -1, SCREENTIMEOUT, MOTIONDETECTION };

	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Settings", FONT_SIZE_TITLE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == SCREENTIMEOUT) {
		sprintf(buffer, "Screen Timeout: %ds\0", temporary_setting);
	}
	else {
		sprintf(buffer, "Screen Timeout: %ds\0", settings->screenTimeoutSec);
	}
	// Draw a screen timeout at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Switch between edited value and set value
	if (edit_oled_menu && oled_menu_item == MOTIONDETECTION) {
		sprintf(buffer, "Away Time: %d hr\0", temporary_setting);
	}
	else {
		// Also, covnert seconds to hours
		sprintf(buffer, "Away Time: %d hr\0", settings->motionDetectorSec / 3600);
	}
	// Draw away time at line 2
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Confine the scroll line of the menu
	boundScrollCounter(1, 2, 9);

	// Save which item we are editing
	oled_menu_item = items[oled_scroll_counter];

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void displayCycle()
{
	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Cycle Info", FONT_SIZE_TITLE, white_pixel);

	// Draw Start time on line 1
	sprintf(buffer, "Start time %02d:%02d\0", settings->currentCycle->start_hour, settings->currentCycle->start_min);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Draw runtime on line 2
	sprintf(buffer, "Runtime %d min\0", furnaceRunTime / 60);
	sd1306_draw_string(OLED_LINE_2_X, OLED_LINE_2_Y, buffer, FONT_SIZE_LINE, white_pixel);

	// Send the buffer to OLED RAM
	sd1306_refresh();
}

void displayDateAndTime() {
	struct timespec currentTime;
	if (clock_gettime(CLOCK_REALTIME, &currentTime) == -1) {
		Log_Debug("ERROR: clock_gettime failed with error code: %s (%d).\n", strerror(errno),
			errno);
		return;
	}
	char displayTimeBuffer[26];
	if (!asctime_r((localtime(&currentTime.tv_sec)), (char *restrict) & displayTimeBuffer)) {
		Log_Debug("ERROR: asctime_r failed with error code: %s (%d).\n", strerror(errno),
			errno);
		return;
	}
	// Remove the new line at the end of 'displayTimeBuffer'
	displayTimeBuffer[strlen(displayTimeBuffer) - 5] = '\0';

	// Clear OLED buffer
	clear_oled_buffer();

	// Draw the title
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Time", FONT_SIZE_TITLE, white_pixel);

	// Draw date at line 1
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, displayTimeBuffer, FONT_SIZE_LINE, white_pixel);

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

void boundScrollCounter(int8_t low, int8_t high, uint8_t height) {
	if (oled_scroll_counter > high) {
		oled_scroll_counter = high;
	}
	else if (oled_scroll_counter < low -1) {
		oled_scroll_counter = low -1;
	}
	if (oled_scroll_counter >= low)
		sd1306_invert_line(0, OLED_WIDTH, line_selection[oled_scroll_counter] - 1, line_selection[oled_scroll_counter] + height);
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