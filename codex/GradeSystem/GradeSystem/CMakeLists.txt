cmake_minimum_required(VERSION 3.16)

project(GradeSystem LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GradeSystem WIN32
        main.cpp
        student.c
        file.c
        analysis.c)

target_compile_definitions(GradeSystem PRIVATE UNICODE _UNICODE)

if (MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(MINGW_ROOT "${MINGW_BIN_DIR}" DIRECTORY)

    find_path(EASYX_INCLUDE_DIR easyx.h
            PATHS
            "${MINGW_ROOT}/include"
            "$ENV{EASYX_ROOT}/include")
    find_library(EASYX_LIBRARY
            NAMES easyxw
            PATHS
            "${MINGW_ROOT}/lib"
            "${MINGW_ROOT}/x86_64-w64-mingw32/lib"
            "$ENV{EASYX_ROOT}/lib64")

    if (NOT EASYX_INCLUDE_DIR OR NOT EASYX_LIBRARY)
        message(FATAL_ERROR
                "EasyX was not found. Install EasyX for MinGW or set EASYX_ROOT.")
    endif ()

    target_include_directories(GradeSystem PRIVATE "${EASYX_INCLUDE_DIR}")
    target_link_libraries(GradeSystem PRIVATE
            "${EASYX_LIBRARY}"
            gdi32 ole32 uuid msimg32 imm32 winmm)
else ()
    message(FATAL_ERROR "This EasyX GUI project currently requires MinGW on Windows.")
endif ()

# Source files are UTF-8. Keep execution strings as UTF-8 so CLion's bundled
# MinGW can compile without requiring an extra iconv conversion library.
if (MSVC)
    target_compile_options(GradeSystem PRIVATE
            /source-charset:utf-8
            /execution-charset:utf-8)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(GradeSystem PRIVATE
            -finput-charset=UTF-8
            -fexec-charset=UTF-8)
endif ()

# Copy data.txt to CLion's build/run directory so the program can read it.
configure_file(${CMAKE_SOURCE_DIR}/data.txt
        ${CMAKE_BINARY_DIR}/data.txt COPYONLY)
