# SPDX-License-Identifier: MIT

################################
# Created by lebakassemmerl 2022 #
# E-Mail: hotschi@gmx.at         #
################################

MSP432_DIR = $(ROOT)/periph/msp432

INCLUDES += $(MSP432_DIR)
SRCS += \
	$(MSP432_DIR)/msp432.cpp \
	$(MSP432_DIR)/irq_handlers.cpp

PERIPHERALS += \
	cortexm4f \
	cs \
	dma \
	flctl \
	gpio \
	pcm \
	sysctl \
	usci \
	wdt
