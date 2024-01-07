# mylang

This repository contains the implementation of my own programming language compiler. The compiler is implemented in C and the compiler's target is also C code.

## Compiling the mylang Compiler

To compile the compiler you need to have a C compiler and CMake installed.
The Makefile contains several commands to facilitate buiding and testing the project.
To compile, simply run `make clean build`.

To run the tests, you can run `make test`.

## Usage

The compiler (actually it is still just an intreperter) supports two modes: interactive (from stdin) or normal (from files).

To run in interactive mode simply launch the built executable without any arguments: `make run`.

To run in normal mode, pass a sequence of file paths as arguments: `make run ARGS="./examples/file.txt ./examples/file2.txt"`.