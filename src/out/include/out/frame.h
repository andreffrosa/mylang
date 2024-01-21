#ifndef _FRAME_H_
#define _FRAME_H_

#include "ast/ast.h"

typedef struct Frame {
    unsigned int size;
    int values[];
} Frame;

int printVar(const Symbol* var, const int* value, IOStream* stream);

int printSymbolTable(const SymbolTable* st, const Frame* frame, IOStream* stream);

Frame* newFrame(unsigned int size);

void deleteFrame(Frame** frame);

int getFrameValue(const Frame* frame, unsigned int index);

void setFrameValue(Frame* frame, unsigned int index, int value);

#endif
