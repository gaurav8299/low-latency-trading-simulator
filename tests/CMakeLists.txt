add_executable(test_types test_types.cpp)
target_link_libraries(test_types PRIVATE hft_core GTest::gtest_main)

add_executable(test_order test_order.cpp)
target_link_libraries(test_order PRIVATE hft_core GTest::gtest_main)

add_executable(test_order_book test_order_book.cpp)
target_link_libraries(test_order_book PRIVATE hft_core GTest::gtest_main)

add_executable(test_matching_engine test_matching_engine.cpp)
target_link_libraries(test_matching_engine PRIVATE hft_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_types)
gtest_discover_tests(test_order)
gtest_discover_tests(test_order_book)
gtest_discover_tests(test_matching_engine)
