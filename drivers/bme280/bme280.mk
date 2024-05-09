# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2024 #
# E-Mail: hotschi@gmx.at         #
##################################

BME280_DIR = $(ROOT)/drivers/bme280

SRCS += $(BME280_DIR)/bme280.cpp

INCLUDES += $(BME280_DIR)
