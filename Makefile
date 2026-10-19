BUILD_DIR ?= build
STM32CUBE_F4_DIR ?=

ifeq ($(OS),Windows_NT)
CMAKE_GENERATOR ?= MinGW Makefiles
CMAKE_MAKE_PROGRAM ?= mingw32-make
CMAKE_GENERATOR_ARGS = -G "$(CMAKE_GENERATOR)" -DCMAKE_MAKE_PROGRAM="$(CMAKE_MAKE_PROGRAM)"
endif

.PHONY: all firmware test flash clean

all: firmware

firmware:
	cmake $(CMAKE_GENERATOR_ARGS) -S . -B $(BUILD_DIR)/firmware \
		-DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
		-DSTM32CUBE_F4_DIR="$(STM32CUBE_F4_DIR)" \
		-DBUILD_FIRMWARE=ON -DBUILD_TESTING=OFF
	cmake --build $(BUILD_DIR)/firmware --parallel

test:
	cmake $(CMAKE_GENERATOR_ARGS) -S . -B $(BUILD_DIR)/host -DBUILD_FIRMWARE=OFF -DBUILD_TESTING=ON
	cmake --build $(BUILD_DIR)/host --parallel
	ctest --test-dir $(BUILD_DIR)/host --output-on-failure

flash: firmware
	STM32_Programmer_CLI -c port=SWD -w $(BUILD_DIR)/firmware/firmware.elf -v -rst

clean:
	cmake -E rm -rf $(BUILD_DIR)
