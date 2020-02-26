#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzsphereThermostat C)

# Create executable
add_executable (${PROJECT_NAME} main.c epoll_timerfd_utilities.c HC-SR04.c HDC1080.c linkedList.c msg.c mt3620.c oled.c parson.c schedule.c sd1306.c thermostat.c)
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c curl)

# Add MakeImage post-build command
include (${AZURE_SPHERE_MAKE_IMAGE_FILE})
