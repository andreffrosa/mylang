cmake_minimum_required(VERSION 3.5)
project(mylang VERSION 0.1.0 LANGUAGES C)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Configure Flex and Bison
find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.0 REQUIRED)
set(BISON_FLAGS "--warnings -Wcounterexamples")
flex_target(LEXER "${SRC_DIR}/lexer.l" "${OUT_DIR}/lexer.c" DEFINES_FILE "${OUT_DIR}/lexer.h")
bison_target(PARSER "${SRC_DIR}/parser.y" "${OUT_DIR}/parser.c" DEFINES_FILE "${OUT_DIR}/parser.h" COMPILE_FLAGS ${BISON_FLAGS})
add_flex_bison_dependency(LEXER PARSER)

# Gather Source Files
set(SRC_FILES "${SRC_DIR}/main.c" "${OUT_DIR}/lexer.c" "${OUT_DIR}/parser.c")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${OUT_DIR})

# Option to enable/disable tests
option(BUILD_TESTS "Enable building tests" ON)
if(BUILD_TESTS)
  message("Building tests!")
  include(CTest)
  enable_testing()

  set(UNITY_SOURCE_DIR "dependencies/Unity/src")
  add_library(Unity STATIC ${UNITY_SOURCE_DIR}/unity.c)
  target_include_directories(Unity PUBLIC ${UNITY_SOURCE_DIR})

  add_executable(test_${PROJECT_NAME} src/test.c "${OUT_DIR}/lexer.c" "${OUT_DIR}/parser.c")
  target_link_libraries(test_${PROJECT_NAME} Unity)
  add_test(test_${PROJECT_NAME} test_${PROJECT_NAME})
  target_include_directories(test_${PROJECT_NAME} PRIVATE ${OUT_DIR})
else()
  message("Skipping tests!")
endif()