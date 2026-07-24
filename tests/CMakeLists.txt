include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    SYSTEM
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(${PROJECT_NAME}_tests test_core.cpp)
target_compile_features(${PROJECT_NAME}_tests PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_core gtest_main)

if (ENABLE_STRICT_CHECKS)
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_strict_checks)
endif (ENABLE_STRICT_CHECKS)

if (ENABLE_SANITIZERS)
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_sanitizers)
endif (ENABLE_SANITIZERS)

gtest_discover_tests(${PROJECT_NAME}_tests)
