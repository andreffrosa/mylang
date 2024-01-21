set(MODULE_NAME out)

# Gather Source Files
set(SRC_DIR "./src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/eval/*.c" "${SRC_DIR}/compile/*.c")

# Gather Header Files
set(PUBLIC_HEADERS "./include/${MODULE_NAME}")
set(PRIVATE_HEADERS "${SRC_DIR}")

add_library(${MODULE_NAME} STATIC ${SRC_FILES})
target_include_directories(${MODULE_NAME} PRIVATE ${PRIVATE_HEADERS} PUBLIC ${PUBLIC_HEADERS})
target_link_libraries(${MODULE_NAME} PRIVATE ast)

# Include Unit Tests
include_tests()