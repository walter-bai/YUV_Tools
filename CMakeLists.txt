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

# Resource embedding
set(TEST_DATA_DIR ${CMAKE_CURRENT_LIST_DIR}/test/data)
file(GLOB BIN_FILES "${TEST_DATA_DIR}/*")
if(WIN32)
    # Generate .rc file for Windows
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")
    file(WRITE ${RC_FILE} "// Generated resources file\n")
    foreach(BIN_FILE IN LISTS BIN_FILES)
        get_filename_component(FILE_NAME ${BIN_FILE} NAME)
        file(APPEND ${RC_FILE} "${FILE_NAME} RCDATA \"${BIN_FILE}\"\n")
    endforeach()
    target_sources(${TEST_NAME} PRIVATE ${RC_FILE})
elseif(UNIX)
    # Output header and source files
    set(RESOURCES_HEADER "${CMAKE_CURRENT_BINARY_DIR}/resources.h")
    set(RESOURCES_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp")

    # Begin writing the header and source files
    file(WRITE ${RESOURCES_HEADER} "#pragma once\n#include <map>\n#include <string>\n\n")
    file(WRITE ${RESOURCES_SOURCE} "#include \"resources.h\"\n\n")

    # Declare a struct to hold data pointers
    file(APPEND ${RESOURCES_HEADER} "struct BinaryData {\n    const unsigned char* start;\n    const unsigned char* end;\n};\n\n")
    # Declare the map
    file(APPEND ${RESOURCES_HEADER} "extern const std::map<std::string, BinaryData> g_binaryDataMap;\n\n")

    # Start map definition in the source file
    file(APPEND ${RESOURCES_SOURCE} "const std::map<std::string, BinaryData> g_binaryDataMap = {\n")

    # Process each binary file
    foreach(BIN_FILE IN LISTS BIN_FILES)
        get_filename_component(FILE_NAME ${BIN_FILE} NAME)

        # Add extern declarations to the header file
        file(APPEND ${RESOURCES_HEADER} "extern \"C\" {\n")
        file(APPEND ${RESOURCES_HEADER} "    extern const unsigned char _binary_${FILE_NAME}_start;\n")
        file(APPEND ${RESOURCES_HEADER} "    extern const unsigned char _binary_${FILE_NAME}_end;\n")
        file(APPEND ${RESOURCES_HEADER} "}\n")

        # Append entries to the map in the source file
        file(APPEND ${RESOURCES_SOURCE} "    {\"${FILE_NAME}\", {&_binary_${FILE_NAME}_start, &_binary_${FILE_NAME}_end}},\n")

        set(OBJECT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.o")
        add_custom_command(
            OUTPUT ${OBJECT_FILE}
            WORKING_DIRECTORY ${TEST_DATA_DIR}  # Set the working directory to the test data directory
            COMMAND objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 ${FILE_NAME} ${OBJECT_FILE}
            DEPENDS ${BIN_FILE}
            COMMENT "Embedding ${BIN_FILE} using objcopy"
        )
        target_sources(${TEST_NAME} PRIVATE ${OBJECT_FILE})
    endforeach()

    # Close the map definition in the source file
    file(APPEND ${RESOURCES_SOURCE} "};\n")
    target_sources(${TEST_NAME} PRIVATE ${RESOURCES_SOURCE})
endif()

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
