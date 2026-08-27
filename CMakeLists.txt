cmake_minimum_required(VERSION 3.30)

project(
    kuri
    VERSION 0.1.0
    DESCRIPTION "kuri cpp project template"
    LANGUAGES CXX

)

set(CMAKE_CXX_EXTENSIONS OFF)

# disable C++20 modules scanning
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_STRICT_CHECKS "enable strict checks" OFF)
add_library(${PROJECT_NAME}_strict_checks INTERFACE)
target_compile_options(${PROJECT_NAME}_strict_checks INTERFACE -Wall -Wextra -Wpedantic)

option(ENABLE_SANITIZERS "enable sanitizers" OFF)
add_library(${PROJECT_NAME}_sanitizers INTERFACE)
target_compile_options(${PROJECT_NAME}_sanitizers INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(${PROJECT_NAME}_sanitizers INTERFACE -fsanitize=address,undefined)

add_subdirectory(lib)
add_subdirectory(src)


option(BUILD_TESTING "build testing" OFF)
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif (BUILD_TESTING)

option(BUILD_EXAMPLES "build examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

option(BUILD_SNIPPETS "build standalone snippets" OFF)
if (BUILD_SNIPPETS)
    add_subdirectory(snippets)
endif (BUILD_SNIPPETS)
