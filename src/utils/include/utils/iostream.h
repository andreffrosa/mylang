#ifndef _OUT__IOSTREAM_H_
#define _OUT__IOSTREAM_H_

#include <stdlib.h>
#include <stdio.h>

typedef void IOStream;

IOStream* openIOStreamFromMemmory(char** ptr, size_t* size);

IOStream* openIOStreamFromFile(const FILE* fp);

IOStream* openIOStreamFromStdout();

int IOStreamWritef(const IOStream* s, const char* format, ...);

int IOStreamClose(IOStream** s);

#endif