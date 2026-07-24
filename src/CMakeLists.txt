add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_23)

target_link_libraries(main PRIVATE ${PROJECT_NAME}_core)

if (ENABLE_STRICT_CHECKS)
    target_link_libraries(main PRIVATE ${PROJECT_NAME}_strict_checks)
endif (ENABLE_STRICT_CHECKS)

if (ENABLE_SANITIZERS)
    target_link_libraries(main PRIVATE ${PROJECT_NAME}_sanitizers)
endif (ENABLE_SANITIZERS)
