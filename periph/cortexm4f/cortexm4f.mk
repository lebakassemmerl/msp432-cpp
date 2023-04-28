# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2022 #
# E-Mail: hotschi@gmx.at         #
##################################

CORTEXM4F_DIR = $(ROOT)/periph/cortexm4f

INCLUDES += \
	$(CORTEXM4F_DIR) \
	$(CORTEXM4F_DIR)/nvic \
	$(CORTEXM4F_DIR)/systick

SRCS += \
	$(CORTEXM4F_DIR)/cortexm4f.cpp \
	$(CORTEXM4F_DIR)/systick/systick.cpp \
	$(CORTEXM4F_DIR)/nvic/nvic.cpp \
	$(CORTEXM4F_DIR)/scb/scb.cpp
