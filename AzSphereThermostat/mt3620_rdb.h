/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

   // This header contains the peripheral pinout definitions for the
   // MT3620 Reference Development Board (RDB)

#pragma once
#include "mt3620.h"

// LED 1 Red channel uses GPIO8.
#define MT3620_RDB_LED1_RED MT3620_GPIO8

// LED 1 Green channel uses GPIO9
#define MT3620_RDB_LED1_GREEN MT3620_GPIO9

// LED 1 Blue channel uses GPIO10
#define MT3620_RDB_LED1_BLUE MT3620_GPIO10

// LED 2 Red channel uses GPIO15
#define MT3620_RDB_LED2_RED MT3620_GPIO15

// LED 2 Green channel uses GPIO16
#define MT3620_RDB_LED2_GREEN MT3620_GPIO16

// LED 2 Blue channel uses GPIO17
#define MT3620_RDB_LED2_BLUE MT3620_GPIO17

// LED 3 Red channel uses GPIO18
#define MT3620_RDB_LED3_RED MT3620_GPIO18

// LED 3 Green channel uses GPIO19
#define MT3620_RDB_LED3_GREEN MT3620_GPIO19

// LED 3 Blue channel uses GPIO20
#define MT3620_RDB_LED3_BLUE MT3620_GPIO20

// LED 4 Red channel uses GPIO21
#define MT3620_RDB_LED4_RED MT3620_GPIO21

// LED 4 Green channel uses GPIO22
#define MT3620_RDB_LED4_GREEN MT3620_GPIO22

// LED 4 Blue channel uses GPIO23
#define MT3620_RDB_LED4_BLUE MT3620_GPIO23

// Status LED Red channel uses GPIO45
#define MT3620_RDB_STATUS_LED_RED MT3620_GPIO45

// Status LED Green channel uses GPIO46
#define MT3620_RDB_STATUS_LED_GREEN MT3620_GPIO46

// Status LED Blue channel uses GPIO47
#define MT3620_RDB_STATUS_LED_BLUE MT3620_GPIO47

// Networking LED Red channel uses GPIO48
#define MT3620_RDB_NETWORKING_LED_RED MT3620_GPIO48

// Networking LED Green channel uses GPIO14
#define MT3620_RDB_NETWORKING_LED_GREEN MT3620_GPIO14

// Networking LED Blue channel uses GPIO11
#define MT3620_RDB_NETWORKING_LED_BLUE MT3620_GPIO11

// Button A uses GPIO12
#define MT3620_RDB_BUTTON_A MT3620_GPIO12

// Button B uses GPIO13
#define MT3620_RDB_BUTTON_B MT3620_GPIO13

// GPIO59 is exposed on header 1 pin 3
#define MT3620_RDB_HEADER1_PIN3_GPIO MT3620_GPIO59

// GPIO0 is exposed on header 1 pin 4
#define MT3620_RDB_HEADER1_PIN4_GPIO MT3620_GPIO0

// GPIO56 is exposed on header 1 pin 5
#define MT3620_RDB_HEADER1_PIN5_GPIO MT3620_GPIO56

// GPIO1 is exposed on header 1 pin 6
#define MT3620_RDB_HEADER1_PIN6_GPIO MT3620_GPIO1

// GPIO58 is exposed on header 1 pin 7
#define MT3620_RDB_HEADER1_PIN7_GPIO MT3620_GPIO58

// GPIO2 is exposed on header 1 pin 8
#define MT3620_RDB_HEADER1_PIN8_GPIO MT3620_GPIO2

// GPIO57 is exposed on header 1 pin 9
#define MT3620_RDB_HEADER1_PIN9_GPIO MT3620_GPIO57

// GPIO3 is exposed on header 1 pin 10
#define MT3620_RDB_HEADER1_PIN10_GPIO MT3620_GPIO3

// GPIO60 is exposed on header 1 pin 11
#define MT3620_RDB_HEADER1_PIN11_GPIO MT3620_GPIO60

// GPIO4 is exposed on header 1 pin 12
#define MT3620_RDB_HEADER1_PIN12_GPIO MT3620_GPIO4

// GPIO28 is exposed on header 2 pin 1
#define MT3620_RDB_HEADER2_PIN1_GPIO MT3620_GPIO28

// GPIO26 is exposed on header 2 pin 3
#define MT3620_RDB_HEADER2_PIN3_GPIO MT3620_GPIO26

// GPIO5 is exposed on header 2 pin 4
#define MT3620_RDB_HEADER2_PIN4_GPIO MT3620_GPIO5

// GPIO29 is exposed on header 2 pin 5
#define MT3620_RDB_HEADER2_PIN5_GPIO MT3620_GPIO29

// GPIO6 is exposed on header 2 pin 6
#define MT3620_RDB_HEADER2_PIN6_GPIO MT3620_GPIO6

// GPIO27 is exposed on header 2 pin 7
#define MT3620_RDB_HEADER2_PIN7_GPIO MT3620_GPIO27

// GPIO7 is exposed on header 2 pin 8
#define MT3620_RDB_HEADER2_PIN8_GPIO MT3620_GPIO7

// GPIO30 is exposed on header 2 pin 9
#define MT3620_RDB_HEADER2_PIN9_GPIO MT3620_GPIO30

// GPIO41 is exposed on header 2 pin 11
#define MT3620_RDB_HEADER2_PIN11_GPIO MT3620_GPIO41

// GPIO43 is exposed on header 2 pin 12
#define MT3620_RDB_HEADER2_PIN12_GPIO MT3620_GPIO43

// GPIO42 is exposed on header 2 pin 13
#define MT3620_RDB_HEADER2_PIN13_GPIO MT3620_GPIO42

// GPIO44 is exposed on header 2 pin 14
#define MT3620_RDB_HEADER2_PIN14_GPIO MT3620_GPIO44

// GPIO66 is exposed on header 3 pin 5
#define MT3620_RDB_HEADER3_PIN5_GPIO MT3620_GPIO66

// GPIO67 is exposed on header 3 pin 7
#define MT3620_RDB_HEADER3_PIN7_GPIO MT3620_GPIO67

// GPIO68 is exposed on header 3 pin 9
#define MT3620_RDB_HEADER3_PIN9_GPIO MT3620_GPIO68

// GPIO69 is exposed on header 3 pin 11
#define MT3620_RDB_HEADER3_PIN11_GPIO MT3620_GPIO69

// GPIO70 is exposed on header 3 pin 12
#define MT3620_RDB_HEADER3_PIN12_GPIO MT3620_GPIO70

// GPIO33 is exposed on header 4 pin 5
#define MT3620_RDB_HEADER4_PIN5_GPIO MT3620_GPIO33

// GPIO38 is exposed on header 4 pin 6
#define MT3620_RDB_HEADER4_PIN6_GPIO MT3620_GPIO38

// GPIO31 is exposed on header 4 pin 7
#define MT3620_RDB_HEADER4_PIN7_GPIO MT3620_GPIO31

// GPIO36 is exposed on header 4 pin 8
#define MT3620_RDB_HEADER4_PIN8_GPIO MT3620_GPIO36

// GPIO34 is exposed on header 4 pin 9
#define MT3620_RDB_HEADER4_PIN9_GPIO MT3620_GPIO34

// GPIO39 is exposed on header 4 pin 10
#define MT3620_RDB_HEADER4_PIN10_GPIO MT3620_GPIO39

// GPIO32 is exposed on header 4 pin 11
#define MT3620_RDB_HEADER4_PIN11_GPIO MT3620_GPIO32

// GPIO37 is exposed on header 4 pin 12
#define MT3620_RDB_HEADER4_PIN12_GPIO MT3620_GPIO37

// GPIO35 is exposed on header 4 pin 13
#define MT3620_RDB_HEADER4_PIN13_GPIO MT3620_GPIO35

// GPIO40 is exposed on header 4 pin 14
#define MT3620_RDB_HEADER4_PIN14_GPIO MT3620_GPIO40

// ISU0 I2C is exposed on header 2, pin 1 (SDA) and pin 7 (SCL)
#define MT3620_RDB_HEADER2_ISU0_I2C MT3620_ISU0_I2C

// ISU1 I2C is exposed on header 4, pin 5 (SDA) and pin 11 (SCL)
#define MT3620_RDB_HEADER4_ISU1_I2C MT3620_ISU1_I2C

// ISU2 I2C is exposed on header 4, pin 6 (SDA) and pin 12 (SCL)
#define MT3620_RDB_HEADER4_ISU2_I2C MT3620_ISU2_I2C

// ISU3 I2C is exposed on header 3, pin 9 (SDA) and pin 7 (SCL)
#define MT3620_RDB_HEADER3_ISU3_I2C MT3620_ISU3_I2C

// ISU0 SPI is exposed on header 2, pin 1 (MISO), pin 3 (SCLK), pin 5 (CSA), pin 7 (MOSI) and pin 9 (CSB)
#define MT3620_RDB_HEADER2_ISU0_SPI MT3620_ISU0_SPI

// ISU1 SPI is exposed on header 4, pin 5 (MISO), pin 7 (SCLK), pin 9 (CSA), pin 11 (MOSI) and pin 13 (CSB)
#define MT3620_RDB_HEADER4_ISU1_SPI MT3620_ISU1_SPI

// ISU2 SPI is exposed on header 4, pin 6 (MISO), pin 8 (SCLK), pin 10 (CSA), pin 12 (MOSI) and pin 14 (CSB)
#define MT3620_RDB_HEADER4_ISU2_SPI MT3620_ISU2_SPI

// ISU3 SPI is exposed on header 3, pin 9 (MISO), pin 5 (SCLK), pin 11 (CSA), pin 7 (MOSI) and pin 12 (CSB)
#define MT3620_RDB_HEADER3_ISU3_SPI MT3620_ISU3_SPI

// ISU0 UART is exposed on header 2, pin 1 (RX), pin 3 (TX), pin 5 (CTS), pin 7 (RTS)
#define MT3620_RDB_HEADER2_ISU0_UART MT3620_ISU0_UART

// ISU1 UART is exposed on header 4, pin 5 (RX), pin 7 (TX), pin 9 (CTS), pin 11 (RTS)
#define MT3620_RDB_HEADER4_ISU1_UART MT3620_ISU1_UART

// ISU2 UART is exposed on header 4, pin 6 (RX), pin 8 (TX), pin 10 (CTS), pin 12 (RTS)
#define MT3620_RDB_HEADER4_ISU2_UART MT3620_ISU2_UART

// ISU3 UART is exposed on header 3, pin 9 (RX), pin 5 (TX), pin 11 (CTS), pin 7 (RTS)
#define MT3620_RDB_HEADER3_ISU3_UART MT3620_ISU3_UART

