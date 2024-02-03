# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2021 #
# E-Mail: hotschi@gmx.at         #
##################################

CORE_DIR = $(ROOT)/core

INCLUDES += $(CORE_DIR)

SRCS += $(CORE_DIR)/libc.cpp

SRCS_NOLTO += \
	$(CORE_DIR)/libc_no_lto.cpp \
	$(CORE_DIR)/cm4f.cpp
