include(FetchContent)

FetchContent_Declare(
    Catch2
    SYSTEM
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.11.0
)

FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "test_*.cpp")
add_executable(all_tests ${TEST_SOURCES})
target_link_libraries(all_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(all_tests PRIVATE ${PROJECT_NAME}_fib)
target_link_libraries(all_tests PRIVATE ${PROJECT_NAME}_net)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(all_tests)
