cmake_minimum_required(VERSION 3.16)

project(GradeSystem LANGUAGES C CXX)

if (NOT WIN32 OR NOT MSVC)
    message(FATAL_ERROR "GradeSystem requires Visual Studio 2026 with MSVC on Windows.")
endif ()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GradeSystem WIN32
        src/main.cpp
        src/app_logic.c
        src/student.c
        src/file.c
        src/analysis.c)

target_compile_definitions(GradeSystem PRIVATE UNICODE _UNICODE NOMINMAX)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(EASYX_ARCH_DIR x64)
else ()
    set(EASYX_ARCH_DIR x86)
endif ()

find_path(EASYX_INCLUDE_DIR easyx.h
        PATHS
        "${CMAKE_GENERATOR_INSTANCE}/VC/Auxiliary/VS/include"
        "$ENV{VSINSTALLDIR}/VC/Auxiliary/VS/include"
        "$ENV{EASYX_ROOT}/include"
        NO_DEFAULT_PATH)
find_library(EASYX_LIBRARY
        NAMES EasyXw
        PATHS
        "${CMAKE_GENERATOR_INSTANCE}/VC/Auxiliary/VS/lib/${EASYX_ARCH_DIR}"
        "$ENV{VSINSTALLDIR}/VC/Auxiliary/VS/lib/${EASYX_ARCH_DIR}"
        "$ENV{EASYX_ROOT}/lib/${EASYX_ARCH_DIR}"
        "$ENV{EASYX_ROOT}/lib"
        NO_DEFAULT_PATH)

if (NOT EASYX_INCLUDE_DIR OR NOT EASYX_LIBRARY)
    message(FATAL_ERROR
            "EasyX for Visual Studio was not found. Install EasyX into Visual Studio 2026 or set EASYX_ROOT.")
endif ()

target_include_directories(GradeSystem PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${EASYX_INCLUDE_DIR}")
target_link_libraries(GradeSystem PRIVATE
        "${EASYX_LIBRARY}"
        gdi32 ole32 uuid msimg32 imm32 winmm)
set_target_properties(GradeSystem PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Source files and runtime strings use UTF-8.
target_compile_options(GradeSystem PRIVATE
        /W4
        /permissive-
        /source-charset:utf-8
        /execution-charset:utf-8
        /wd4201)

# Seed the runtime data only for a new build directory. Reconfiguring CMake
# must not overwrite records saved by the application.
if (NOT EXISTS "${CMAKE_BINARY_DIR}/data.txt")
    configure_file("${CMAKE_SOURCE_DIR}/data/data.txt"
            "${CMAKE_BINARY_DIR}/data.txt" COPYONLY)
endif ()
