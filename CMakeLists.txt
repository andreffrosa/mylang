cmake_minimum_required(VERSION 3.5)
project(mylang VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME} src/main.c)

# Option to enable/disable tests
option(BUILD_TESTS "Enable building tests" ON)
if(BUILD_TESTS)
  message("Building tests!")
  include(CTest)
  enable_testing()

  set(UNITY_SOURCE_DIR "dependencies/Unity/src")
  add_library(Unity STATIC ${UNITY_SOURCE_DIR}/unity.c)
  target_include_directories(Unity PUBLIC ${UNITY_SOURCE_DIR})

  add_executable(test_${PROJECT_NAME} src/test.c)
  target_link_libraries(test_${PROJECT_NAME} Unity)
  add_test(test_${PROJECT_NAME} test_${PROJECT_NAME})
else()
  message("Skipping tests!")
endif()