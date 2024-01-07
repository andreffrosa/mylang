#include <unity.h>

void test();

void setUp (void) {}
void tearDown (void) {}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test);
  return UNITY_END();
}

void test() {
  TEST_ASSERT_EQUAL_INT(1, 1);
}
