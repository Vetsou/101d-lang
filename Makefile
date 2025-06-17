##############################
# PROJECT CONFIG
##############################

# BUILD MODES:
# dev     - Standard build
# debug   - Enables debug traces and logs
# release - Builds with all optimizations
BUILD_MODE ?= dev

# FILE PATH
# Use this to determine the input file for the compiler
FILE_PATH ?= example/parser_test.101d

##############################

# Paths
SRC_DIR = src
VENDOR_DIR = vendor
TEST_DIR = test
OBJ_DIR = build

# Setup variables for build modes
ifeq ($(BUILD_MODE),debug)
	EXTRA_CFLAGS = -DDEBUG_TRACE_EXEC
else ifeq ($(BUILD_MODE),release)
	EXTRA_CFLAGS = -O3 -DNDEBUG
	OBJ_DIR = build-release
else
	EXTRA_CFLAGS =
endif

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
APP_SRCS = $(filter-out $(SRC_DIR)/main.c, $(SRCS))
APP_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRCS))
MAIN_OBJ = $(OBJ_DIR)/main.o

DEPS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.d, $(SRCS))
TARGET = $(OBJ_DIR)/101D

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -I$(SRC_DIR)/include -m64 -fanalyzer -Wall -Wextra -Wno-format $(EXTRA_CFLAGS)
TEST_CFLAGS = -std=c99 -I$(SRC_DIR)/include -I$(VENDOR_DIR) -m64 -fanalyzer -Wall -Wextra -Wno-format

-include $(DEPS)

##############################
# SETUP
##############################

.PHONY: setup clean

setup:
ifeq ($(OS),Windows_NT)
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
else
	@mkdir -p "$(OBJ_DIR)"
endif

clean:
ifeq ($(OS),Windows_NT)
	@if exist build rmdir /S /Q build
	@if exist build-release rmdir /S /Q build-release
else
	@rm -rf build build-release
endif

##############################
# DEV BUILD COMMANDS
##############################

.PHONY: build run

build: setup $(TARGET)

run: setup $(TARGET)
	$(TARGET) $(FILE_PATH)

##############################
# RELEASE BUILD COMMANDS
##############################

.PHONY: build-release run-release

build-release: clean
	$(MAKE) BUILD_MODE=release build

run-release: clean
	$(MAKE) BUILD_MODE=release run

##############################
# DEBUG BUILD COMMANDS
##############################

.PHONY: build-debug run-debug

build-debug: clean
	$(MAKE) BUILD_MODE=debug build

run-debug: clean
	$(MAKE) BUILD_MODE=debug run

##############################
# PROJECT BUILD RULES
##############################

# Target
$(TARGET): $(APP_OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

##############################
# TEST MODULES
##############################

TEST_OBJ = build-test

# Unity testing lib rules
UNITY_SRC = $(VENDOR_DIR)/unity.c
UNITY_OBJ = $(TEST_OBJ)/unity.o

$(UNITY_OBJ): $(UNITY_SRC)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

.PHONY: test-setup clean-test

test-setup:
ifeq ($(OS),Windows_NT)
	@if not exist "$(TEST_OBJ)" mkdir "$(TEST_OBJ)"
else
	@mkdir -p "$(TEST_OBJ)"
endif

clean-test:
ifeq ($(OS),Windows_NT)
	@if exist build-test rmdir /S /Q build-test
else
	@rm -rf build-test
endif

# Testing module names
TEST_MODULES = lexer chunk compiler

# TEST TEMPLATE ############################################
define TEST_TEMPLATE
TEST_SRCS_$(1) = $(TEST_DIR)/$(1)_tests.c
TEST_OBJS_$(1) = $(TEST_OBJ)/$(1)_tests.o
TEST_TARGET_$(1) = $(TEST_OBJ)/test_$(1)

.PHONY: test-$(1) run-test-$(1)

test-$(1): test-setup build $$(TEST_TARGET_$(1))

run-test-$(1): test-setup build test-$(1)
	$$(TEST_TARGET_$(1))

$$(TEST_OBJS_$(1)): $$(TEST_SRCS_$(1))
	$$(CC) $$(TEST_CFLAGS) -c $$< -o $$@

$$(TEST_TARGET_$(1)): $$(TEST_OBJS_$(1)) $$(APP_OBJS) $$(UNITY_OBJ)
	$$(CC) $$(TEST_CFLAGS) -o $$@ $$^
endef
############################################################

$(foreach module,$(TEST_MODULES),$(eval $(call TEST_TEMPLATE,$(module))))

build-all-tests: $(foreach m,$(TEST_MODULES),test-$(m))
run-all-tests: $(foreach m,$(TEST_MODULES),run-test-$(m))