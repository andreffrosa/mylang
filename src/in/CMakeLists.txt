set(MODULE_NAME in)

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
set(SRC_DIR "./src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")
list(APPEND SRC_FILES "${OUT_DIR}/lexer.c" "${OUT_DIR}/parser.c")

#message("Source files: ${SRC_FILES}")

# Gather Header Files
set(PUBLIC_HEADERS "./include/${MODULE_NAME}")
file(GLOB PRIVATE_HEADERS "${SRC_DIR}/*.h")
list(APPEND PRIVATE_HEADERS "${OUT_DIR}/lexer.h" "${OUT_DIR}/parser.h")

add_library(${MODULE_NAME} STATIC ${SRC_FILES} ${PRIVATE_HEADERS})
target_include_directories(${MODULE_NAME} PRIVATE ${OUT_DIR} PUBLIC ${PUBLIC_HEADERS})
target_link_libraries(${MODULE_NAME} PRIVATE ast)

# Include Unit Tests
include_tests()