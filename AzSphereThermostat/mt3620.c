#include "mt3620.h"

int ISU_2_Fd = -1;
int GPIO_relay_Fd = -1;
int motionDetectorGpioFd = -1;
// rotary encoder
int rotary_A_Fd = -1;
int rotary_B_Fd = -1;
int rotary_SW_Fd = -1;
GPIO_Value_Type rotary_A_LastState = GPIO_Value_High;
GPIO_Value_Type rotary_B_LastState = GPIO_Value_High;
GPIO_Value_Type rotary_SW_LastState = GPIO_Value_High;
GPIO_Value_Type rotary_A_State = GPIO_Value_High;
GPIO_Value_Type rotary_B_State = GPIO_Value_High;
GPIO_Value_Type rotary_SW_State = GPIO_Value_High;

// buttons
int buttonAGpioFd = -1;
int buttonBGpioFd = -1;
GPIO_Value_Type buttonAState = GPIO_Value_High;
GPIO_Value_Type buttonBState = GPIO_Value_High;

int8_t oled_scroll_counter = 0;
int8_t oled_menu_state = 0;
int8_t oled_menu_item = -1;
int8_t temporary_setting = 0;
bool edit_oled_menu = false;
bool oledScreenON = true;
bool reconfigureTimer = false;

long lastMotionDetectedTimeStamp = 0;
