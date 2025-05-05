cmake_minimum_required(VERSION 3.21)

# Conditional compiler setting
if(APPLE)
    # Looking for gcc installed by Homebrew
    find_program(GCC NAMES gcc-14 gcc-13 gcc-12 gcc-11)
    find_program(GXX NAMES g++-14 g++-13 g++-12 g++-11)
    if(GCC AND GXX)
        set(CMAKE_C_COMPILER ${GCC})
        set(CMAKE_CXX_COMPILER ${GXX})
    else()
        message(FATAL_ERROR "No suitable gcc/g++ compiler found. Please install gcc and g++.")
    endif()
elseif(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Project name and version
project(APImageFilters VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add the executable
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

add_executable(APImageFilters
    src/main.cpp
    src/Filter.cpp
    src/Image.cpp
    src/Projection.cpp
    src/Slice.cpp
    src/Volume.cpp
    ${HEADER_FILES}
)

set(TEST_SOURCES
    Tests/Test_main.cpp
    Tests/Test.cpp
    src/Filter.cpp
    src/Image.cpp
    src/Volume.cpp  # Added to resolve Volume symbols
    src/Projection.cpp
    src/Slice.cpp
)

# Add the test executable
add_executable(runUnitTests
    ${TEST_SOURCES}
    ${HEADER_FILES}
)

# Enable testing
include(CTest)
enable_testing()

add_test(NAME UnitTests COMMAND runUnitTests)

# Include the tests
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)
