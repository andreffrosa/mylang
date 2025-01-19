#include <assert.h>

#include "ast/ast.h"

#include "frame.h"

int printVar(const Symbol* var, const int* value, IOStream* stream) {
    assert(var != NULL);
    assert(stream != NULL);

    /*if (!isVarInitialized(var) || value == NULL) {
        return IOStreamWritef(stream, "%s %s = -", ASTTypeToStr(getVarType(var)), getVarId(var));
    }*/

    char buffer[TYPE_VALUE_BUFFER_SIZE];
    ASTTypeValueToStr(getVarType(var), *value, buffer);
    return IOStreamWritef(stream, "%s %s = %s", ASTTypeToStr(getVarType(var)), getVarId(var), buffer);
}

int printSymbolTable(const SymbolTable* st, const Frame* frame, IOStream* stream) {
    assert(st != NULL);
    assert(frame != NULL);
    assert(stream != NULL);

    unsigned int var_count = getMaxOffset(st) + 1;

    int n_bytes = IOStreamWritef(stream, " (%d vars) [", var_count);
    if(var_count > 0) {
        Symbol* var = NULL;
        unsigned int i = 0;
        for(; i < var_count - 1; i++) {
            var = lookupLastVarWithOffset(st, i);
            assert(var != NULL);
            if(frame != NULL) {
                int value = getFrameValue(frame, i);
                n_bytes += printVar(var, &value, stream);
            } else {
                n_bytes += printVar(var, NULL, stream);
            }
            n_bytes += IOStreamWritef(stream, ", ");
        }
        if(var_count > 0) {
            var = lookupLastVarWithOffset(st, i);
            assert(var != NULL);
            if(frame != NULL) {
                int value = getFrameValue(frame, i);
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
