set(MODULE_NAME utils)

# Gather Source Files
set(SRC_DIR "./src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

# Gather Header Files
set(PUBLIC_HEADERS "./include/${MODULE_NAME}")
set(PRIVATE_HEADERS ${SRC_DIR})

# Add Static Library Config
add_library(${MODULE_NAME} STATIC ${SRC_FILES})
target_include_directories(${MODULE_NAME} PRIVATE ${PRIVATE_HEADERS} PUBLIC ${PUBLIC_HEADERS})

# Include Unit Tests
include_tests()
