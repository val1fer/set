include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  set_test
  set_ut.cpp
)

target_link_libraries(
  set_test
  GTest::gtest_main
  GTest::gmock_main
  set
)

target_include_directories(set_test PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(set_test)
