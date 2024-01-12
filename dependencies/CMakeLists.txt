set(MODULE_NAME dependencies)

if(BUILD_TESTS)
  set(UNITY_SOURCE_DIR "Unity/src")
  add_library(Unity STATIC ${UNITY_SOURCE_DIR}/unity.c)
  target_include_directories(Unity PUBLIC ${UNITY_SOURCE_DIR})
endif()
