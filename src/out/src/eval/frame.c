#include <assert.h>

#include "ast/ast.h"

#include "frame.h"

int printVar(const Symbol* var, const int* value, IOStream* stream) {
    assert(var != NULL && stream != NULL);

    if (!isVarInitialized(var) || value == NULL) {
        return IOStreamWritef(stream, "%s %s = -", ASTTypeToStr(getVarType(var)), getVarId(var));
    }

    char buffer[TYPE_VALUE_BUFFER_SIZE];
    ASTTypeValueToStr(getVarType(var), *value, buffer);
    return IOStreamWritef(stream, "%s %s = %s", ASTTypeToStr(getVarType(var)), getVarId(var), buffer);
}

int printSymbolTable(const SymbolTable* st, const Frame* frame, IOStream* stream) {
    assert(st != NULL && stream != NULL);

    int n_vars = getSymbolTableSize(st);

    int n_bytes = IOStreamWritef(stream, " (%d vars) [", n_vars);
    if(n_vars > 0) {
        Symbol* var = NULL;
        int i = 0;
        for(; i < n_vars - 1; i++) {
            var = getVarFromIndex(st, i);
            if(frame != NULL) {
                unsigned int index = getVarIndex(st, var);
                int value = getFrameValue(frame, index);
                n_bytes += printVar(var, &value, stream);
            } else {
                n_bytes += printVar(var, NULL, stream);
            }
            n_bytes += IOStreamWritef(stream, ", ");
        }
        if(n_vars > 0) {
            var = getVarFromIndex(st, i);
            if(frame != NULL) {
                unsigned int index = getVarIndex(st, var);
                int value = getFrameValue(frame, index);
                n_bytes += printVar(var, &value, stream);
            } else {
                n_bytes += printVar(var, NULL, stream);
            }
        }
    }
    n_bytes += IOStreamWritef(stream, "]\n");
    return n_bytes;
}

Frame* newFrame(unsigned int size) {
    Frame* f = (Frame*) malloc(sizeof(Frame) + size * sizeof(int));
    f->size = size;
    return f;
}

void deleteFrame(Frame** frame) {
    assert(frame != NULL && *frame != NULL);
    free(*frame);
    *frame = NULL;
}

int getFrameValue(const Frame* frame, unsigned int index) {
    assert(frame != NULL && index < frame->size);
    return frame->values[index];
}

void setFrameValue(Frame* frame, unsigned int index, int value) {
    assert(frame != NULL && index < frame->size);
    frame->values[index] = value;
}
