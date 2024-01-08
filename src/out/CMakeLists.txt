set(MODULE_NAME out)

# Gather Source Files
set(SRC_DIR "./src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

# Gather Header Files
set(PUBLIC_HEADERS "./include/${MODULE_NAME}")
file(GLOB PRIVATE_HEADERS "${SRC_DIR}/*.h")

add_library(${MODULE_NAME} STATIC ${SRC_FILES})
target_include_directories(${MODULE_NAME} PRIVATE ${OUT_DIR} PUBLIC ${PUBLIC_HEADERS})
target_link_libraries(${MODULE_NAME} PRIVATE ast)

# Include Unit Tests
include_tests()