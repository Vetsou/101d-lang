# Paths
SRC_DIR = src
OBJ_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
APP_SRCS = $(filter-out $(SRC_DIR)/main.c, $(SRCS))
APP_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRCS))
MAIN_OBJ = $(OBJ_DIR)/main.o

DEPS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.d, $(SRCS))
TARGET = $(OBJ_DIR)/101D

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -I$(SRC_DIR)/include -m64 -fanalyzer -Wall -Wextra -Wno-format

##############################
# PROJECT BUILD RULES
##############################

.PHONY: build run setup

-include $(DEPS)

setup:
ifeq ($(OS),Windows_NT)
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
else
	@mkdir -p "$(OBJ_DIR)"
endif

build: setup $(TARGET)

run: setup $(TARGET)
	$(TARGET)

# Target
$(TARGET): $(APP_OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
