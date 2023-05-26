# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2023 #
# E-Mail: hotschi@gmx.at         #
##################################

ROOT = ../..
PROJ_NAME = button-test
PROJ_DIR = $(ROOT)/projects/button-test
BUILD_DIR = $(PROJ_DIR)/build
OBJ_DIR = $(BUILD_DIR)/obj
HEX_DIR = $(BUILD_DIR)/hex

PERIPHERALS += \
	msp432

DRIVERS += \
	button \
	event_timer \
	led

INCLUDES += $(PROJ_DIR)
SRCS += $(wildcard $(PROJ_DIR)/*.cpp)

include $(ROOT)/makefile.mk
