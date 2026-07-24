add_executable(example_core example_core.cpp)
target_compile_features(example_core PRIVATE cxx_std_23)
target_link_libraries(example_core PRIVATE ${PROJECT_NAME}_core)

if (ENABLE_STRICT_CHECKS)
    target_link_libraries(example_core PRIVATE ${PROJECT_NAME}_strict_checks)
endif (ENABLE_STRICT_CHECKS)

if (ENABLE_SANITIZERS)
    target_link_libraries(example_core PRIVATE ${PROJECT_NAME}_sanitizers)
endif (ENABLE_SANITIZERS)
