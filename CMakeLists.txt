cmake_minimum_required (VERSION 3.16.3)

# Main
set (MAIN_NAME yuv_tools)
project (${MAIN_NAME})

file (GLOB MAIN_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

file (GLOB MAIN_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
    ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
)

source_group (${MAIN_NAME} FILES ${MAIN_SOURCES} ${MAIN_HEADERS})
add_executable (${MAIN_NAME} ${MAIN_SOURCES} ${MAIN_HEADERS})

# Test
set (TEST_NAME unit_test)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # Set timestamp to time of extraction
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file (GLOB TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/test/*.c
    ${CMAKE_CURRENT_LIST_DIR}/test/*.cpp
)

file (GLOB TEST_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/test/*.h
    ${CMAKE_CURRENT_LIST_DIR}/test/*.hpp
)

source_group (${TEST_NAME} FILES ${TEST_SOURCES} ${TEST_HEADERS})
add_executable (${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(${TEST_NAME} gtest_main)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
