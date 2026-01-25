add_executable(unit_tests
  test_main.cpp
  test_cli.cpp
  test_core.cpp
  ${CMAKE_SOURCE_DIR}/src/cli.cpp
  ${CMAKE_SOURCE_DIR}/src/core.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main spdlog::spdlog)

include(GoogleTest)
gtest_discover_tests(unit_tests)