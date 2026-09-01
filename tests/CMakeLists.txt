# ==============================================================================
# Tests CMakeLists.txt
# ==============================================================================
#
# WHAT: Tells CMake how to build and register our test executables.
# WHY:  Automated tests catch bugs early. Every phase adds more tests.
# HOW:  cmake --build . -j && ctest --output-on-failure
#
# ==============================================================================

# --------------------------------------------------------------------------
# test_types — Tests for our core type definitions
# --------------------------------------------------------------------------
# Each test executable links against:
#   - hft_core:       Our simulator library (the code we wrote)
#   - GTest::gtest_main: Google Test framework (provides main() for tests)
# --------------------------------------------------------------------------
add_executable(test_types test_types.cpp)
target_link_libraries(test_types PRIVATE hft_core GTest::gtest_main)

# Register with CTest so "ctest" discovers and runs it
# gtest_discover_tests automatically finds all TEST() macros
include(GoogleTest)
gtest_discover_tests(test_types)
