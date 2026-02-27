add_library(${PROJECT_NAME}_fib lib/fib.cc)
target_compile_features(${PROJECT_NAME}_fib PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME}_fib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(main bin/main.cc)
target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE ${PROJECT_NAME}_lib)
target_link_libraries(main PRIVATE ${PROJECT_NAME}_fib)
