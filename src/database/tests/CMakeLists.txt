add_executable(DatabaseTests
  connectionTests.cpp
  queryTests.cpp)
target_link_libraries(DatabaseTests
  database
  gmock_main
)
gtest_discover_tests(DatabaseTests)