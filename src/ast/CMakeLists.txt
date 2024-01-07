set(MODULE_NAME ast)

# Gather Source Files
set(SRC_DIR "./src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

# Gather Header Files
set(PUBLIC_HEADERS "./include/${MODULE_NAME}")
file(GLOB PRIVATE_HEADERS "${SRC_DIR}/*.h")

# Add Static Library Config
add_library(${MODULE_NAME} STATIC ${SRC_FILES} ${PRIVATE_HEADERS})
#set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME})
target_include_directories(${MODULE_NAME} PUBLIC ${PUBLIC_HEADERS})

# Include Unit Tests
include_tests()
