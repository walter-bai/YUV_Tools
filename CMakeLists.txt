cmake_minimum_required (VERSION 3.16.3)

# Main

set (NAME_ yuv_tools)

project (${NAME_})

set (
    SOURCES_
    ""
)
file (GLOB SOURCES_
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c,
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

set (
    HEADERS_
    ""
)
file (GLOB HEADERS_
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
    ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
)

source_group (${NAME_} FILES ${SOURCES_} ${HEADERS_})

add_executable (${NAME_} ${SOURCES_} ${HEADERS_})

# Test

set (NAME_ unit_test)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set (
    SOURCES_
    ""
)
file (GLOB SOURCES_
    ${CMAKE_CURRENT_LIST_DIR}/test/*.c,
    ${CMAKE_CURRENT_LIST_DIR}/test/*.cpp
)

set (
    HEADERS_
    ""
)
file (GLOB HEADERS_
    ${CMAKE_CURRENT_LIST_DIR}/test/*.h
    ${CMAKE_CURRENT_LIST_DIR}/test/*.hpp
)

source_group (${NAME_} FILES ${SOURCES_} ${HEADERS_})

add_executable (${NAME_} ${SOURCES_} ${HEADERS_})
target_link_libraries(${NAME_} gtest_main)
add_test(NAME ${NAME_} COMMAND ${NAME_})
