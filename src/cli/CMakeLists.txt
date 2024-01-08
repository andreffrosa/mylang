set(MODULE_NAME cli)

set(SRC_DIR ".")

add_executable(${PROJECT_NAME} "${SRC_DIR}/main.c")
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE in out)