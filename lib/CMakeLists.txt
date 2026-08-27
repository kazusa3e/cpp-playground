file (
    GLOB core_sources
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(${PROJECT_NAME}_core STATIC ${core_sources})
target_compile_features(${PROJECT_NAME}_core PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME}_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (ENABLE_STRICT_CHECKS)
    target_link_libraries(${PROJECT_NAME}_core PRIVATE ${PROJECT_NAME}_strict_checks)
endif (ENABLE_STRICT_CHECKS)

if (ENABLE_SANITIZERS)
    target_link_libraries(${PROJECT_NAME}_core PRIVATE ${PROJECT_NAME}_sanitizers)
endif (ENABLE_SANITIZERS)
