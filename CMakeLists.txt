# ==============================================================================
# HFT-Style Trading Simulator — Root CMakeLists.txt
# ==============================================================================
#
# WHAT: This file tells CMake how to build our entire project.
# WHY:  CMake is a "build system generator" — it creates the actual build files
#        (Makefiles on Linux, Visual Studio projects on Windows) from this
#        single cross-platform description.
# HOW:  mkdir build && cd build && cmake .. && cmake --build . -j
#
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

project(hft-simulator
    VERSION 0.1.0
    DESCRIPTION "HFT-Style Trading Simulator for Educational Use"
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# C++20 Standard — required for modern features we'll use throughout
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # Fail if compiler doesn't support C++20
set(CMAKE_CXX_EXTENSIONS OFF)          # Don't use GNU extensions, stay portable

# ------------------------------------------------------------------------------
# Compiler warnings — catch bugs early (applied to our targets only)
# ------------------------------------------------------------------------------
# We define warning flags here but apply them only to our own targets (hft_core,
# hft_simulator) below, NOT globally. This prevents warnings from firing on
# third-party code like GoogleTest.
#
# -Wall:    Enable most warnings
# -Wextra:  Enable even more warnings
# -Wpedantic: Warn about non-standard C++ usage
# -Werror=return-type: Error if a function forgets to return a value
# -Wconversion: Warn about implicit type conversions
# -Wsign-conversion: Warn about sign changes in conversions
# ------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(HFT_WARNING_FLAGS
        -Wall -Wextra -Wpedantic -Werror=return-type -Wconversion -Wsign-conversion
    )
elseif(MSVC)
    set(HFT_WARNING_FLAGS /W4 /permissive-)
endif()

# ------------------------------------------------------------------------------
# Sanitizers — automatic bug detectors (Debug builds only)
# ------------------------------------------------------------------------------
# AddressSanitizer:  Detects memory errors (buffer overflow, use-after-free)
# UndefinedBehavior: Detects undefined behavior (signed overflow, null deref)
#
# These make the program slower but catch bugs that would otherwise be silent.
# We only enable them in Debug mode, not Release.
#
# NOTE: Sanitizers are NOT available on MinGW/Windows. They require a native
# Linux/macOS GCC or Clang. On Windows we skip them (tests still catch bugs).
# ------------------------------------------------------------------------------
set(HFT_SANITIZER_FLAGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MINGW AND NOT WIN32)
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer (Debug)")
        set(HFT_SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
    else()
        message(STATUS "Sanitizers not available on this platform (MinGW/Windows). Skipping.")
    endif()
endif()

# ------------------------------------------------------------------------------
# Include directories — where the compiler looks for our .hpp header files
# ------------------------------------------------------------------------------
# ${CMAKE_SOURCE_DIR} is the root of our project (where this CMakeLists.txt is).
# So "include/common/types.hpp" becomes findable as <common/types.hpp>.
# ------------------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/include)

# ------------------------------------------------------------------------------
# Collect source files
# ------------------------------------------------------------------------------
# GLOB_RECURSE finds all .cpp files in the src/ directory tree.
# We separate main.cpp so test builds can use the library without main().
# ------------------------------------------------------------------------------
file(GLOB_RECURSE LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
)

# Create a static library with shared code (everything except main)
# This lets tests link against the same code without including main().
add_library(hft_core STATIC ${LIB_SOURCES})
target_include_directories(hft_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(hft_core PRIVATE ${HFT_WARNING_FLAGS} ${HFT_SANITIZER_FLAGS})
target_link_options(hft_core PRIVATE ${HFT_SANITIZER_FLAGS})

# Create the main executable
add_executable(hft_simulator ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(hft_simulator PRIVATE hft_core)
target_compile_options(hft_simulator PRIVATE ${HFT_WARNING_FLAGS} ${HFT_SANITIZER_FLAGS})
target_link_options(hft_simulator PRIVATE ${HFT_SANITIZER_FLAGS})

# ------------------------------------------------------------------------------
# Testing with Google Test
# ------------------------------------------------------------------------------
# FetchContent downloads GoogleTest automatically — no manual installation needed.
# This is the standard way to add dependencies in modern CMake.
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Prevent GoogleTest from overriding our compiler settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable CTest (the test runner)
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# ------------------------------------------------------------------------------
# Print build summary
# ------------------------------------------------------------------------------
message(STATUS "")
message(STATUS "=== HFT-Style Trading Simulator ===")
message(STATUS "Version:      ${PROJECT_VERSION}")
message(STATUS "Build type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "===================================")
message(STATUS "")
