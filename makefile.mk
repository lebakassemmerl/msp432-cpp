# SPDX-License-Identifier: MIT

##################################
# Created by lebakassemmerl 2021 #
# E-Mail: hotschi@gmx.at         #
##################################

OUTPUT_FILE = $(HEX_DIR)/$(PROJ_NAME)
OPENOCD = openocd
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = size

# clang is not supported (yet)
ifeq ($(COMPILER),clang)
	CXX = clang++ --target=armv6m-none-eabi -stdlib=libstdc++
	LD = clang++ --target=armv6m-none-eabi -stdlib=libstdc++
else
	CXX = arm-none-eabi-g++
	LD = arm-none-eabi-g++
endif

OPENOCD_OPTIONS = -f $(ROOT)/openocd.cfg

TARGET_OPTIONS := \
	-mthumb \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

CXXFLAGS := \
	-std=c++2b \
	-ffunction-sections \
	-fdata-sections \
	-fno-use-cxa-atexit \
	-fno-threadsafe-statics \
	-nostdlib \
	-ffreestanding \
	-fno-rtti \
	-nostartfiles \
	-nodefaultlibs \
	-fno-exceptions \
	-fno-unwind-tables \
	-fconcepts-diagnostics-depth=6 \
	-Wdouble-promotion \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wno-unused-parameter

LDFLAGS := \
	-Wl,--start-group \
	-lgcc \
	-Wl,--end-group \
	-T $(ROOT)/layout.ld \

# add debug or release specific compiler and linker options
ifeq ($(RELEASE),1)
	CXXFLAGS_NOLTO += $(CXXFLAGS) -O2 -fno-lto
	CXXFLAGS += -O2 -flto
	LDFLAGS  += -flto
else
	CXXFLAGS_NOLTO += $(CXXFLAGS) -O0 -fno-lto
	CXXFLAGS += -O0 -g
endif

PERIPHERALS ?=
DRIVERS ?=

# the core modules are always built
# TODO: maybe we split the core modules into different makefiles if there will more modules which
# require a .cpp file
include $(ROOT)/core/core.mk

# some peripherals have to be compiled always since they are needed at startup
PERIPHERALS += flctl pcm sysctl wdt

# include the core-, preiph- and driver-makefiles twice since this enables the possibility to add
# other core- and driver-modules within the makefile of a core- or driver-module, e.g. the UART
# driver could require the DMA driver just by mentioning it in the CORE_MODULES variable

# stage 1: include the makefiles of the modules from the makefile of the project
_PERIPHERALS_STAGE1 = $(sort $(PERIPHERALS))
_DRIVERS_STAGE1 = $(sort $(DRIVERS))

# include the necessary peripheral makefiles
MAKEFILES_STAGE1 += $(foreach periph, $(_PERIPHERALS_STAGE1), $(ROOT)/periph/$(periph)/$(periph).mk)
MAKEFILES_STAGE1 += $(foreach driver, $(_DRIVERS_STAGE1), $(ROOT)/drivers/$(driver)/$(driver).mk)
include $(MAKEFILES_STAGE1)

# stage 2: include the makefiles of periph- and driver-modules again since the variables
# PERIPH_MODULES and DRIVER_MODULES could have been extended by a module
_PERIPHERALS_STAGE2 += $(sort $(PERIPHERALS))
_DRIVERS_STAGE2 += $(sort $(DRIVERS))
MAKEFILES_STAGE2 += $(foreach periph, $(_PERIPHERALS_STAGE2), $(ROOT)/periph/$(periph)/$(periph).mk)
MAKEFILES_STAGE2 += $(foreach driver, $(_DRIVERS_STAGE2), $(ROOT)/drivers/$(driver)/$(driver).mk)

# include the peripherals first since the drivers are maybe dependent on them
include $(MAKEFILES_STAGE2)

# the startup file is always necessary, so include it here for all projects
SRCS += $(ROOT)/startup.cpp
MK_DEFS += -DARM_MATH_CM4=1

# get rid of multiple entries and add the -I flag to the includes
_SRCS  = $(sort $(SRCS))
_SRCS_NOLTO = $(sort $(SRCS_NOLTO))
_INCLUDES = $(sort $(INCLUDES))
_INCLUDES := $(foreach inc, $(_INCLUDES), -I $(inc))
_MK_DEFS := $(sort $(MK_DEFS))

OBJS :=

# function to define a target for every source file
define cpp_goal
$2: $1
	@echo -- compiling $(notdir $1)
	@$(CXX) $(TARGET_OPTIONS) $(CXXFLAGS) $(_INCLUDES) $(_MK_DEFS) -c $1 -o $2
OBJS += $2
endef

define cpp_nolto_goal
$2: $1
	@echo -- compiling $(notdir $1) without LTO
	@$(CXX) $(TARGET_OPTIONS) $(CXXFLAGS_NOLTO) $(_INCLUDES) $(_MK_DEFS) -c $1 -o $2
OBJS += $2
endef

${info --}
${info -- Project: $(PROJ_NAME)}
${info -- Branch:  $(shell git branch --show-current)}
${info --}

.DEFAULT_GOAL  := all

# generate a target for each cpp-file
$(foreach cppfile, $(_SRCS), $(eval $(call cpp_goal, \
	$(cppfile), $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(cppfile))))))
$(foreach cppfile, $(_SRCS_NOLTO), $(eval $(call cpp_nolto_goal, \
	$(cppfile), $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(cppfile))))))

all: prebuild postbuild

rebuild: clean all

postbuild: $(OUTPUT_FILE).elf $(OUTPUT_FILE).hex $(OUTPUT_FILE).lss
	@echo --
	@echo -- size of $(PROJ_NAME):
	@$(SIZE) --format=berkeley $(OUTPUT_FILE).elf
	@echo --
	@echo -- finished compiling and linking!

$(OUTPUT_FILE).elf: $(OBJS)
	@echo --
	@echo -- linking $(PROJ_NAME)
	@$(LD) $(TARGET_OPTIONS) $(CXXFLAGS) $(_MK_DEFS) -o $(OUTPUT_FILE).elf $(OBJS) $(LDFLAGS)

$(OUTPUT_FILE).hex: $(OUTPUT_FILE).elf
	@$(OBJCOPY) -O ihex $(OUTPUT_FILE).elf $(OUTPUT_FILE).hex
	@echo -- generated hex file!

$(OUTPUT_FILE).lss: $(OUTPUT_FILE).elf
	@echo -- creating lss file
	@$(OBJDUMP) -d $(OUTPUT_FILE).elf > $(OUTPUT_FILE).lss

.PHONY: prebuild
prebuild:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(HEX_DIR)

.PHONY: clean
clean:
	@echo -- clean project:
	@echo -- remove directory $(BUILD_DIR)
	@echo --
	@rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(OUTPUT_FILE).elf
	$(OPENOCD) $(OPENOCD_OPTIONS) -c "init; reset halt; flash write_image erase $<; verify_image $<; reset; shutdown"
