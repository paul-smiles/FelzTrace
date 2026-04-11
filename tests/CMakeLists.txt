add_executable(unit_tests
  test_main.cpp
  test_cli.cpp
  test_core.cpp
  test_requirement_store_yml.cpp
  test_real_filesystem.cpp
  ${CMAKE_SOURCE_DIR}/src/cli.cpp
  ${CMAKE_SOURCE_DIR}/src/core.cpp
  ${CMAKE_SOURCE_DIR}/src/requirement_store_yml.cpp
  ${CMAKE_SOURCE_DIR}/src/real_filesystem.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main GTest::gmock spdlog::spdlog yaml-cpp)

include(GoogleTest)
gtest_discover_tests(unit_tests)