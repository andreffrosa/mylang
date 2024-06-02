# Disable Print Directory
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

.PHONY: build build-win debug run test clean cov

all: clean build run

build:
#	cmake -S . -B build -DCMAKE_VERBOSE_MAKEFILE=ON
	cmake -S . -B build
	cmake --build build --clean-first

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE:STRING=Debug
	cmake --build build --clean-first

cov: 
	cmake -S . -B build-cov -DCMAKE_BUILD_TYPE:STRING=Debug -DBUILD_COV=ON
	cmake --build build-cov --clean-first
	cmake --build build-cov --target coverage

run: debug
	./build/src/cli/mylang $(ARGS)

test: debug
	ctest --test-dir build --output-on-failure

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(ARGS)

clean:
	rm -rf build*

build-win:
	cmake -S . -B build-win -DCMAKE_TOOLCHAIN_FILE=.toolchains/win-x64-static.cmake
	cmake --build build-win --clean-first
