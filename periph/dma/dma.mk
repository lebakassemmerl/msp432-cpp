# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2022 #
# E-Mail: hotschi@gmx.at         #
##################################

DMA_DIR = $(ROOT)/periph/dma

INCLUDES += $(DMA_DIR)
SRCS += \
	$(DMA_DIR)/dma.cpp \
	$(DMA_DIR)/dma_channel.cpp
