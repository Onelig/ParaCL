include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_pass test.cpp)
target_link_libraries(test_pass PRIVATE GTest::gtest_main definitions_lib)

add_test(MainTest test_pass)