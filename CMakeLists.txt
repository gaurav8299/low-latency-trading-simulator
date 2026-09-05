cmake_minimum_required(VERSION 3.20)

project(hft-simulator
    VERSION 0.1.0
    DESCRIPTION "Low-Latency Trading Simulator"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(HFT_WARNING_FLAGS
        -Wall -Wextra -Wpedantic -Werror=return-type -Wconversion -Wsign-conversion
    )
elseif(MSVC)
    set(HFT_WARNING_FLAGS /W4 /permissive-)
endif()

# Sanitizers (Debug builds on non-Windows platforms)
set(HFT_SANITIZER_FLAGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MINGW AND NOT WIN32)
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        set(HFT_SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
    ${CMAKE_SOURCE_DIR}/src/orderbook/*.cpp
)

# Core Library
add_library(hft_core STATIC ${LIB_SOURCES})
target_include_directories(hft_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(hft_core PRIVATE ${HFT_WARNING_FLAGS} ${HFT_SANITIZER_FLAGS})
target_link_options(hft_core PRIVATE ${HFT_SANITIZER_FLAGS})

# Main Executable
add_executable(hft_simulator ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(hft_simulator PRIVATE hft_core)
target_compile_options(hft_simulator PRIVATE ${HFT_WARNING_FLAGS} ${HFT_SANITIZER_FLAGS})
target_link_options(hft_simulator PRIVATE ${HFT_SANITIZER_FLAGS})

# Testing
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)
