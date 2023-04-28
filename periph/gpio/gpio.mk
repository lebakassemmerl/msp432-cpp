# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2021 #
# E-Mail: hotschi@gmx.at         #
##################################

GPIO_DIR = $(ROOT)/periph/gpio

INCLUDES += $(GPIO_DIR)
SRCS += $(GPIO_DIR)/pin.cpp \
		$(GPIO_DIR)/int_pin.cpp
