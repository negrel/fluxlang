TARGET_LIB ?= lib$(shell basename $(PROJECT_DIR)).a

export FLUX_CMD

BUILD_DIR ?= ./dist
CMD_DIR ?= ./cmd
INC_DIR ?= ./inc


INCS := $(shell find $(INC_DIR) -regex '.+\.h')
CMD_SRCS := $(shell find $(CMD_DIR)/$(FLUX_CMD) -regex '.+\.c')

CC ?= clang
CFLAGS ?=
CFLAGS := $(CFLAGS) -Wall -Wextra -I$(INC_DIR) -I$$PWD

AR := ar

MKDIR := mkdir
MKDIR_P := mkdir -p
