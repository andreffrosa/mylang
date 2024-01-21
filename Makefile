# Disable Print Directory
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

.PHONY: build build-win run test clean

all: clean build run

build:
#	cmake -S . -B build -DCMAKE_VERBOSE_MAKEFILE=ON
	cmake -S . -B build
	cmake --build build --clean-first

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE:STRING=Debug
	cmake --build build --clean-first

run:
	./build/src/cli/mylang $(ARGS)

test:
#	cmake -S . -B build -DBUILD_TESTS=ON
#	cmake --build build --clean-first
	ctest --test-dir build --output-on-failure

valgrind:
#	cmake -S . -B build -DCMAKE_BUILD_TYPE:STRING=Debug
#	cmake --build build --clean-first
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(ARGS)

clean:
	rm -rf build*

build-win:
	cmake -S . -B build-win -DCMAKE_TOOLCHAIN_FILE=.toolchains/win-x64-static.cmake
	cmake --build build-win --clean-first
