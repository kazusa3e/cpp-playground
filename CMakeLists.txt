cmake_minimum_required(VERSION 4.0.0)

project(
    CPP_PLAYGROUND
    VERSION 0.1.0
    DESCRIPTION "A C++ project to play around with C++ features"
    LANGUAGES CXX
)

# disable cxx20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# global template library
add_library(${PROJECT_NAME}_lib INTERFACE)
target_compile_features(${PROJECT_NAME}_lib INTERFACE cxx_std_23)
target_include_directories(${PROJECT_NAME}_lib INTERFACE ${CMAKE_SOURCE_DIR}/include)

# enable sanitizer check
add_library(${PROJECT_NAME}_sanitizer INTERFACE)
# set(SANITIZER_FLAGS "-fsanitize=address,undefined,thread,memory,leak -fno-omit-frame-pointer")
set(SANITIZER_FLAGS -fsanitize=address,undefined,leak -fno-omit-frame-pointer)
target_compile_options(${PROJECT_NAME}_sanitizer INTERFACE ${SANITIZER_FLAGS})
target_link_options(${PROJECT_NAME}_sanitizer INTERFACE ${SANITIZER_FLAGS})

# enable strict warnings
add_library(${PROJECT_NAME}_strict_warnings INTERFACE)
# set(STRICT_WARNINGS_FLAGS -Wall -Wextra -Wpedantic -Werror)
set(STRICT_WARNINGS_FLAGS -Wall -Wextra -Werror)
target_compile_options(${PROJECT_NAME}_strict_warnings INTERFACE ${STRICT_WARNINGS_FLAGS})

option(ENABLE_SANITIZER "Enable sanitizer checks" OFF)
if (ENABLE_SANITIZER)
    message(STATUS "Sanitizer checks are enabled.")
    link_libraries(${PROJECT_NAME}_sanitizer)
endif(ENABLE_SANITIZER)

option(ENABLE_STRICT_WARNINGS "Enable strict warnings" OFF)
if (ENABLE_STRICT_WARNINGS)
    message(STATUS "Strict warnings are enabled.")
    link_libraries(${PROJECT_NAME}_strict_warnings)
endif(ENABLE_STRICT_WARNINGS)

add_subdirectory(src)

option(ENABLE_TESTING "Enable testing" OFF)
if (ENABLE_TESTING)
    message(STATUS "Testing is enabled.")
    enable_testing()
    add_subdirectory(tests)
endif(ENABLE_TESTING)
