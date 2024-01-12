#include <string.h>

#include <unity.h>

#include "iostream.h"

void setUp (void) {}
void tearDown (void) {}

void testIOStreamFromMemorySingleLine() {
    char* ptr;
    size_t size;
    IOStream* s = openIOStreamFromMemmory(&ptr, &size);

    int n = IOStreamWritef(s, "%s\n", "Hello World");

    IOStreamClose(&s);

    TEST_ASSERT_EQUAL_INT(n+1, size);
    TEST_ASSERT_EQUAL_INT(strlen("Hello World")+1, n);
    TEST_ASSERT_EQUAL_STRING("Hello World\n", ptr);

    free(ptr);
}

void testIOStreamFromMemoryMultipleLines() {
    char* ptr;
    size_t size;
    IOStream* s = openIOStreamFromMemmory(&ptr, &size);

    const char* expected_str = "Hello World\nLine2\n";
    size_t expected_size = strlen(expected_str)+1;

    int n = IOStreamWritef(s, "%s\n", "Hello World");
    n += IOStreamWritef(s, "%s\n", "Line2");

    IOStreamClose(&s);

    TEST_ASSERT_EQUAL_INT(expected_size, n+1);
    TEST_ASSERT_EQUAL_INT(expected_size, size);
    TEST_ASSERT_EQUAL_STRING(expected_str, ptr);

    free(ptr);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(testIOStreamFromMemorySingleLine);
    RUN_TEST(testIOStreamFromMemoryMultipleLines);
    return UNITY_END();
}
