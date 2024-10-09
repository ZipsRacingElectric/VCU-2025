# TODO(Barach): Different targets for debug and standard (-O levels for cortex-debug)

# Project name
PROJECT = vcu_2025

# Imported files
CHIBIOS  := $(CHIBIOS_SOURCE_PATH)

# Directories
CONFDIR  := ./config
BUILDDIR := ./build
DEPDIR   := ./build/dep
BOARDDIR := ./build/board

# Include
ALLINC += $(CURDIR)/src

# Source files
CSRC =	$(ALLCSRC)						\
		src/main.c						\
		src/debug.c						\
		src/can_thread.c				\
		src/peripherals.c				\
		src/peripherals/analog.c		\
		src/peripherals/eeprom.c		\
		src/peripherals/pedals.c		\
		src/can/can_node.c				\
		src/can/ecumaster_gps_v2.c		\
		src/can/misc_node.c

# Common library includes
include common/make/peripherals/mc24lc32.mk

# Compiler flags
USE_OPT = -O0

# C macro definitions
UDEFS = -DCHPRINTF_USE_FLOAT=1

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