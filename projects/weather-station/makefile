# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2024 #
# E-Mail: hotschi@gmx.at         #
##################################

ROOT = ../..
PROJ_NAME = weather-station
PROJ_DIR = $(ROOT)/projects/weather-station
BUILD_DIR = $(PROJ_DIR)/build
OBJ_DIR = $(BUILD_DIR)/obj
HEX_DIR = $(BUILD_DIR)/hex

PERIPHERALS += \
	msp432

DRIVERS += \
	bme280 \
	i2c \
	led \
	uart \

INCLUDES += $(PROJ_DIR)
SRCS += $(wildcard $(PROJ_DIR)/*.cpp)

include $(ROOT)/makefile.mk
