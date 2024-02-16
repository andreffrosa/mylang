#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "iostream.h"

struct MemBuffer {
    char** ptr;
    size_t* buffer_size;
    char* buffer;
    size_t available_size;
};

void initMemBuffer(struct MemBuffer* mb, char** ptr, size_t* size) {
    assert(mb != NULL);
    mb->ptr = ptr;
    *ptr = NULL;
    mb->buffer_size = size;
    *size = 0;
    mb->buffer = NULL;
    mb->available_size = 0;
}

struct MemBuffer* newMemBuffer(char** ptr, size_t* size) {
    struct MemBuffer* mb = malloc(sizeof(struct MemBuffer));
    initMemBuffer(mb, ptr, size);
    return mb;
}

int MemBufferWritef(struct MemBuffer* mb, const char* format, va_list args) {
    if(mb == NULL || format == NULL) {
        return -1;
    }

    va_list args_1, args_2;
    va_copy(args_1, args);
    va_copy(args_2, args);

    size_t required_chars = vsnprintf(NULL, 0, format, args_1);
    if(mb->buffer == NULL) {
        mb->buffer = malloc(required_chars + 1);
        *(mb->ptr) = mb->buffer;
        *(mb->buffer_size) = required_chars + 1;
        mb->available_size = required_chars;  // Do not count the \0
    } else if(mb->available_size < required_chars) {
        size_t extra = (required_chars - mb->available_size);
        size_t new_size = *(mb->buffer_size) + extra;
        size_t offset = *(mb->ptr) - mb->buffer;
        mb->buffer = realloc(mb->buffer, new_size); // may merge with the if
        *(mb->buffer_size) = new_size;
        mb->available_size += extra;
        assert(mb->available_size == required_chars);
        *(mb->ptr) = mb->buffer + offset;
    }

    size_t written_chars = vsnprintf(*(mb->ptr), mb->available_size + 1, format, args_2);
    *(mb->ptr) += written_chars;
    mb->available_size -= written_chars;

    va_end(args_1);
    va_end(args_2);

    return written_chars;
}

int MemBufferClose(struct MemBuffer* mb) {
    *(mb->ptr) = mb->buffer;
    free(mb);
    return 0;
}

typedef int (*IOStreamWritter)(const void* handler, const char* format, va_list args);

typedef int (*IOStreamCloser)(const void* handler);

struct IOStream {
    const void* state;
    IOStreamWritter write;
    IOStreamCloser close;
};


IOStream* newIOStream(const void* state, const IOStreamWritter write, const IOStreamCloser close) {
    struct IOStream* s = malloc(sizeof(struct IOStream));
    s->state = state;
    s->write = write;
    s->close = close;
    return s;
}

IOStream* openIOStreamFromMemmory(char** ptr, size_t* size) {
    struct MemBuffer* mb = newMemBuffer(ptr, size);
    return newIOStream(mb, (IOStreamWritter)&MemBufferWritef, (IOStreamCloser)&MemBufferClose);
}

IOStream* openIOStreamFromFile(const FILE* fp) {
    return newIOStream(fp, (IOStreamWritter)&vfprintf, (IOStreamCloser)&fclose);
}

int IOStreamWritef(const IOStream* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    struct IOStream* s = (struct IOStream*) stream;
    int written = s->write(s->state, format, args);

    va_end(args);
    return written;
}

int IOStreamClose(IOStream** stream) {
    struct IOStream* s = (struct IOStream*) *stream;
    int status = 0;
    if(s->close != NULL) {
        status = s->close(s->state);
    }
    s->state = NULL;
    s->write = NULL;
    s->close = NULL;
    free(s);
    *stream = NULL;
    return status;
}

IOStream* openIOStreamFromStdout() {
    return newIOStream(stdout, (IOStreamWritter)&vfprintf, NULL);
}