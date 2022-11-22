cmake_minimum_required (VERSION 3.16.3)

set (NAME_ yuv_tools)

project (${NAME_})

set (
    SOURCES_
    ""
)
file (GLOB SOURCES_
    ${CMAKE_CURRENT_LIST_DIR}/*.c,
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

set (
    HEADERS_
    ""
)
file (GLOB HEADERS_
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

source_group (${NAME_} FILES ${SOURCES_} ${HEADERS_})

add_executable (${NAME_} ${SOURCES_} ${HEADERS_})
