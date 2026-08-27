file(
    GLOB example_sources
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(source IN LISTS example_sources)
    get_filename_component(example_name "${source}" NAME_WE)
    string(MAKE_C_IDENTIFIER "${example_name}" example_target)

    add_executable("${example_target}" "${source}")
    set_target_properties("${example_target}" PROPERTIES OUTPUT_NAME "${example_name}")
    target_compile_features("${example_target}" PRIVATE cxx_std_23)
    target_link_libraries("${example_target}" PRIVATE ${PROJECT_NAME}_core)

    if (ENABLE_STRICT_CHECKS)
        target_link_libraries("${example_target}" PRIVATE ${PROJECT_NAME}_strict_checks)
    endif (ENABLE_STRICT_CHECKS)

    if (ENABLE_SANITIZERS)
        target_link_libraries("${example_target}" PRIVATE ${PROJECT_NAME}_sanitizers)
    endif (ENABLE_SANITIZERS)
endforeach()

# Register dependencies required only by individual example targets below.
