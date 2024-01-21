cmake_minimum_required(VERSION 3.5)
project(mylang VERSION 0.1.0 LANGUAGES C)

# Force CMake to display all messages
set(CMAKE_MESSAGE_LOG_LEVEL TRACE)

# Set binary output directory
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

# Configure GCC warning and error flags
set(GCC_FLAGS "-Wall -Wextra -Werror -Werror=implicit-int -Werror=implicit-function-declaration -Werror=strict-prototypes")
set(CMAKE_CXX_FLAGS ${GCC_FLAGS})

# Include directories for header files
file(GLOB INCLUDE_DIRECTORIES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/*/include")
include_directories(${INCLUDE_DIRECTORIES})

# Option to enable/disable tests
option(BUILD_TESTS "Enable building tests" ON)
if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  message("Tests Enabled!")
else()
  message("Tests Disabled!")
endif()

# Function to include tests of a module
function(include_tests)
  if(BUILD_TESTS)
    message("Setting-up tests for module ${MODULE_NAME}")
    set(TEST_DIR "./test")
    file(GLOB TEST_SOURCES "${TEST_DIR}/*.c")
    foreach(test_file ${TEST_SOURCES})
      get_filename_component(test_name ${test_file} NAME_WE)
      set(test_target "test_${MODULE_NAME}_${test_name}")
      message("Including test: ${test_target}")

      add_executable(${test_target} ${test_file})
      set_target_properties(${test_target} PROPERTIES OUTPUT_NAME ${test_target})
      target_link_libraries(${test_target} Unity ${MODULE_NAME})
      target_include_directories(${test_target} PRIVATE ${SRC_DIR} ${PUBLIC_HEADERS} ${OUT_DIR})
      add_test(NAME ${test_target} COMMAND ${test_target})
    endforeach()
  endif()
endfunction()

function(include_test file)
  if(BUILD_TESTS)
    set(TEST_DIR "./test")
    set(test_file "${TEST_DIR}/${file}")
    get_filename_component(test_name ${test_file} NAME_WE)
    set(test_target "test_${MODULE_NAME}_${test_name}")
    message("Including test: ${test_target}")

    if(${ARGC} GREATER 1)
      list(TRANSFORM ARGV1 PREPEND "${TEST_DIR}/")
    endif()

    add_executable(${test_target} ${test_file} ${ARGV1})
    set_target_properties(${test_target} PROPERTIES OUTPUT_NAME ${test_target})
    target_link_libraries(${test_target} Unity ${MODULE_NAME})
    target_include_directories(${test_target} PRIVATE ${SRC_DIR} ${PUBLIC_HEADERS} ${OUT_DIR})
    add_test(NAME ${test_target} COMMAND ${test_target})
  endif()
endfunction()


# Include sub-modules
add_subdirectory(dependencies)
add_subdirectory(src/utils)
add_subdirectory(src/ast)
add_subdirectory(src/in)
add_subdirectory(src/out)
add_subdirectory(src/cli)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)



