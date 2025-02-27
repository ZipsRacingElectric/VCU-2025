# Project name
PROJECT = vcu_2025

# Imported files
CHIBIOS  := $(CHIBIOS_SOURCE_PATH)

# Directories
CONFDIR  := ./config
BUILDDIR := ./build
DEPDIR   := ./build/dep
BOARDDIR := ./build/board

# Includes
ALLINC += src

# Source files
CSRC =	$(ALLCSRC)							\
		src/main.c							\
											\
		src/peripherals.c					\
		src/peripherals/am4096.c			\
		src/peripherals/eeprom_map.c		\
		src/peripherals/pedals.c			\
		src/peripherals/steering_angle.c	\
											\
		src/can_thread.c					\
		src/can/receive.c					\
		src/can/transmit.c					\
											\
		src/torque_thread.c					\
		src/controls/tv_chatfield.c			\
		src/controls/tv_straight_diff.c		\
											\
		src/state_thread.c

# Common library includes
include common/src/debug.mk
include common/src/fault_handler.mk

include common/src/peripherals/analog.mk
include common/src/peripherals/eeprom.mk
include common/src/peripherals/linear_sensor.mk
include common/src/peripherals/mc24lc32.mk

include common/src/can/amk_inverter.mk
include common/src/can/can_node.mk
include common/src/can/ecumaster_gps_v2.mk
include common/src/can/eeprom_can.mk

include common/src/controls/pid_controller.mk

# Compiler flags
USE_OPT = -Og -Wall -Wextra -lm

# C macro definitions
UDEFS =

# ASM definitions
UADEFS =

# Include directories
UINCDIR =

# Library directories
ULIBDIR =

# Libraries
ULIBS =

# ChibiOS extra includes
include $(CHIBIOS)/os/hal/lib/streams/streams.mk

# Common toolchain includes
include common/makefile

# ChibiOS compilation hooks
PRE_MAKE_ALL_RULE_HOOK: $(BOARD_FILES) $(CLANGD_FILE)