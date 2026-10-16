CC := arm-none-eabi-gcc
HOST_CC := gcc
CFLAGS := -Wall -Wextra -Os -std=c11
SRC := $(wildcard src/*.c)

TARGET_DIR := build
TARGET := $(TARGET_DIR)/main.elf

TEST_SRCS := test/test_led.c src/led.c
TEST_BIN := $(TARGET_DIR)/test_runner

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -ffreestanding -fdata-sections -ffunction-sections \
		-nostdlib -Wl,--gc-sections -Wl,-e,main -o $@ $(SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS) src/led.h
	mkdir -p $(TARGET_DIR)
	$(HOST_CC) $(CFLAGS) -Isrc -o $@ $(TEST_SRCS)

clean:
	rm -rf $(TARGET_DIR)
