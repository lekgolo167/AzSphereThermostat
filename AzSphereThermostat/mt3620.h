/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

   // This header lists the available peripherals for the Mediatek MT3620
   // and provides the header definition and application manifest values required to use them.

#pragma once
#include <applibs/gpio.h>
#include <unistd.h>
#include <stdbool.h>
#include "epoll_timerfd_utilities.h"
#include "thermostat.h"
#include "schedule.h"
#include "HDC1080.h"
#include "HC-SR04.h"
#include "oled.h"
#include "msg.h"

struct HDC1080* HDC1080_sensor_ptr;
struct thermostatSettings* userSettings_ptr;

extern int ISU_2_Fd;
extern int GPIO_relay_Fd;
extern int motionDetectorGpioFd;
extern int furnaceRelayStateFd;
extern int rotary_A_Fd;
extern int rotary_B_Fd;
extern int rotary_SW_Fd;

// Timer file descriptors
static int epollFd = -1;
static int buttonPollTimerFd = -1;
static int sensorPollTimerFd = -1;
static int motionPollTimerFd = -1;
static int schedulePollTimerFd = -1;
static int temporaryPollTimerFd = -1;

extern GPIO_Value_Type rotary_A_LastState;
extern GPIO_Value_Type rotary_B_LastState;
extern GPIO_Value_Type rotary_SW_LastState;

extern int buttonAGpioFd;
extern int buttonBGpioFd;

extern GPIO_Value_Type lastBtnAState;
extern GPIO_Value_Type lastBtnBState;

extern int8_t oled_scroll_counter;
extern int8_t oled_menu_state;
extern int8_t oled_menu_item;
extern int8_t temporary_setting;
extern bool edit_oled_menu;
extern bool oledScreenON;

extern bool reconfigureTimer;
extern bool startTemporaryTimer;

extern bool CURL_enabled;

extern long lastMotionDetectedTimeStamp;
extern long furnaceStartTime;
extern long furnaceRunTime;

// global char buffer for curl messages
char* CURLMessageBuffer[128];

// IP address of server
#define IP_ADDRESS_AND_PORT "192.168.0.27:1880"

#define URL_TEMPERATURE IP_ADDRESS_AND_PORT "/temp"
#define URL_STATS IP_ADDRESS_AND_PORT "/stats"
#define URL_MOTION IP_ADDRESS_AND_PORT "/motion"
#define URL_RUNTIME IP_ADDRESS_AND_PORT "/runtime"
#define URL_FURNANCE_STATE IP_ADDRESS_AND_PORT "/furnaceState"

/// <summary>
///     Handler for the rotary encoder input and buttons A and B
/// </summary>
void ButtonTimerEventHandler(EventData* eventData);

/// <summary>
///     Handler for sensor timer to read temperature and humidity then update the OLED with optional CURL to server
/// </summary>
void SensorTimerEventHandler(EventData* eventData);

/// <summary>
///     Handler for motion timer to read motion sensor
/// </summary>
void MotionTimerEventHandler(EventData* eventData);

/// <summary>
///     Handler for schedule update check timer to update the scheudle from the server
/// </summary>
void ScheduleServerCheckEventHandler(EventData* eventData);

void TemporaryTimerEventHandler(EventData* eventData);

// event handler data structures. Only the event handler field needs to be populated.
static EventData buttonEventData = { .eventHandler = &ButtonTimerEventHandler };
static EventData sensorEventData = { .eventHandler = &SensorTimerEventHandler };
static EventData motionEventData = { .eventHandler = &MotionTimerEventHandler };
static EventData scheduleEventData = { .eventHandler = &ScheduleServerCheckEventHandler };
static EventData temporaryEventData = { .eventHandler = &TemporaryTimerEventHandler };

/// <summary>
///     Update the sensor timer if it has been reconfigured
/// </summary>
int reconfigureSensorEpollTimer();

int startTemporaryEpollTimer();

/// <summary>
///     Initializes all of the timer event handlers
/// </summary>
int initTimerEventHandlers();

// MT3620 GPIO 0
#define MT3620_GPIO0 (0)

// MT3620 GPIO 1
#define MT3620_GPIO1 (1)

// MT3620 GPIO 2
#define MT3620_GPIO2 (2)

// MT3620 GPIO 3
#define MT3620_GPIO3 (3)

// MT3620 GPIO 4
#define MT3620_GPIO4 (4)

// MT3620 GPIO 5
#define MT3620_GPIO5 (5)

// MT3620 GPIO 6
#define MT3620_GPIO6 (6)

// MT3620 GPIO 7
#define MT3620_GPIO7 (7)

// MT3620 GPIO 8
#define MT3620_GPIO8 (8)

// MT3620 GPIO 9
#define MT3620_GPIO9 (9)

// MT3620 GPIO 10
#define MT3620_GPIO10 (10)

// MT3620 GPIO 11
#define MT3620_GPIO11 (11)

// MT3620 GPIO 12
#define MT3620_GPIO12 (12)

// MT3620 GPIO 13
#define MT3620_GPIO13 (13)

// MT3620 GPIO 14
#define MT3620_GPIO14 (14)

// MT3620 GPIO 15
#define MT3620_GPIO15 (15)

// MT3620 GPIO 16
#define MT3620_GPIO16 (16)

// MT3620 GPIO 17
#define MT3620_GPIO17 (17)

// MT3620 GPIO 18
#define MT3620_GPIO18 (18)

// MT3620 GPIO 19
#define MT3620_GPIO19 (19)

// MT3620 GPIO 20
#define MT3620_GPIO20 (20)

// MT3620 GPIO 21
#define MT3620_GPIO21 (21)

// MT3620 GPIO 22
#define MT3620_GPIO22 (22)

// MT3620 GPIO 23
#define MT3620_GPIO23 (23)

// MT3620 GPIO 26. Pin shared with ISU0
#define MT3620_GPIO26 (26)

// MT3620 GPIO 27. Pin shared with ISU0
#define MT3620_GPIO27 (27)

// MT3620 GPIO 28. Pin shared with ISU0
#define MT3620_GPIO28 (28)

// MT3620 GPIO 29. Pin shared with ISU0
#define MT3620_GPIO29 (29)

// MT3620 GPIO 30. Pin shared with ISU0
#define MT3620_GPIO30 (30)

// MT3620 GPIO 31. Pin shared with ISU1
#define MT3620_GPIO31 (31)

// MT3620 GPIO 32. Pin shared with ISU1
#define MT3620_GPIO32 (32)

// MT3620 GPIO 33. Pin shared with ISU1
#define MT3620_GPIO33 (33)

// MT3620 GPIO 34. Pin shared with ISU1
#define MT3620_GPIO34 (34)

// MT3620 GPIO 35. Pin shared with ISU1
#define MT3620_GPIO35 (35)

// MT3620 GPIO 36. Pin shared with ISU2
#define MT3620_GPIO36 (36)

// MT3620 GPIO 37. Pin shared with ISU2
#define MT3620_GPIO37 (37)

// MT3620 GPIO 38. Pin shared with ISU2
#define MT3620_GPIO38 (38)

// MT3620 GPIO 39. Pin shared with ISU2
#define MT3620_GPIO39 (39)

// MT3620 GPIO 40. Pin shared with ISU2
#define MT3620_GPIO40 (40)

// MT3620 GPIO 41
#define MT3620_GPIO41 (41)

// MT3620 GPIO 42
#define MT3620_GPIO42 (42)

// MT3620 GPIO 43
#define MT3620_GPIO43 (43)

// MT3620 GPIO 44
#define MT3620_GPIO44 (44)

// MT3620 GPIO 45
#define MT3620_GPIO45 (45)

// MT3620 GPIO 46
#define MT3620_GPIO46 (46)

// MT3620 GPIO 47
#define MT3620_GPIO47 (47)

// MT3620 GPIO 48
#define MT3620_GPIO48 (48)

// MT3620 GPIO 56
#define MT3620_GPIO56 (56)

// MT3620 GPIO 57
#define MT3620_GPIO57 (57)

// MT3620 GPIO 58
#define MT3620_GPIO58 (58)

// MT3620 GPIO 59
#define MT3620_GPIO59 (59)

// MT3620 GPIO 60
#define MT3620_GPIO60 (60)

// MT3620 GPIO 61
#define MT3620_GPIO61 (61)

// MT3620 GPIO 62
#define MT3620_GPIO62 (62)

// MT3620 GPIO 63
#define MT3620_GPIO63 (63)

// MT3620 GPIO 64
#define MT3620_GPIO64 (64)

// MT3620 GPIO 65
#define MT3620_GPIO65 (65)

// MT3620 GPIO 66. Pin shared with ISU3
#define MT3620_GPIO66 (66)

// MT3620 GPIO 67. Pin shared with ISU3
#define MT3620_GPIO67 (67)

// MT3620 GPIO 68. Pin shared with ISU3
#define MT3620_GPIO68 (68)

// MT3620 GPIO 69. Pin shared with ISU3
#define MT3620_GPIO69 (69)

// MT3620 GPIO 70. Pin shared with ISU3
#define MT3620_GPIO70 (70)

// MT3620 GPIO 71. Pin shared with ISU4
#define MT3620_GPIO71 (71)

// MT3620 GPIO 72. Pin shared with ISU4
#define MT3620_GPIO72 (72)

// MT3620 GPIO 73. Pin shared with ISU4
#define MT3620_GPIO73 (73)

// MT3620 GPIO 74. Pin shared with ISU4
#define MT3620_GPIO74 (74)

// MT3620 GPIO 75. Pin shared with ISU4
#define MT3620_GPIO75 (75)

// MT3620 GPIO 80
#define MT3620_GPIO80 (80)

// MT3620 ISU 0 configured as I2C
#define MT3620_ISU0_I2C (0)

// MT3620 ISU 1 configured as I2C
#define MT3620_ISU1_I2C (1)

// MT3620 ISU 2 configured as I2C
#define MT3620_ISU2_I2C (2)

// MT3620 ISU 3 configured as I2C
#define MT3620_ISU3_I2C (3)

// MT3620 ISU 4 configured as I2C
#define MT3620_ISU4_I2C (4)

// MT3620 ISU 0 configured as SPI
#define MT3620_ISU0_SPI (0)

// MT3620 ISU 1 configured as SPI
#define MT3620_ISU1_SPI (1)

// MT3620 ISU 2 configured as SPI
#define MT3620_ISU2_SPI (2)

// MT3620 ISU 3 configured as SPI
#define MT3620_ISU3_SPI (3)

// MT3620 ISU 4 configured as SPI
#define MT3620_ISU4_SPI (4)

// MT3620 ISU 0 configured as UART
#define MT3620_ISU0_UART (4)

// MT3620 ISU 1 configured as UART
#define MT3620_ISU1_UART (5)

// MT3620 ISU 2 configured as UART
#define MT3620_ISU2_UART (6)

// MT3620 ISU 3 configured as UART
#define MT3620_ISU3_UART (7)

// MT3620 ISU 4 configured as UART
#define MT3620_ISU4_UART (8)

// MT3620 SPI CS A, it must not be used as a peripheral in app_manifest
#define MT3620_SPI_CS_A (-1)

// MT3620 SPI CS B, it must not be used as a peripheral in app_manifest
#define MT3620_SPI_CS_B (-2)

