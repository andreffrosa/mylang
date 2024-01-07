# Disable Print Directory
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

.PHONY: build run test clean

all: clean build run

build:
#	cmake -S . -B build -DCMAKE_VERBOSE_MAKEFILE=ON
	cmake -S . -B build
	cmake --build build --clean-first

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE:STRING=Debug
	cmake --build build --clean-first

run:
	./build/src/cli/calc

test:
#	cmake -S . -B build -DBUILD_TESTS=ON
#	cmake --build build --clean-first
	ctest --test-dir build --output-on-failure

clean:
	rm -rf build*
