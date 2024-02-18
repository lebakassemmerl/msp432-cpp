# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2021 #
# E-Mail: hotschi@gmx.at         #
##################################

ROOT = ../..
PROJ_NAME = lcd-test
PROJ_DIR = $(ROOT)/projects/lcd-test
BUILD_DIR = $(PROJ_DIR)/build
OBJ_DIR = $(BUILD_DIR)/obj
HEX_DIR = $(BUILD_DIR)/hex

PERIPHERALS += \
	msp432

DRIVERS += \
	led \
	spi \
	uart \
	w2812b \
	us_timer \

INCLUDES += $(PROJ_DIR)
SRCS += $(wildcard $(PROJ_DIR)/*.cpp)

include $(ROOT)/makefile.mk
