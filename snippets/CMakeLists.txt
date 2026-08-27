file(
    GLOB snippet_sources
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(source IN LISTS snippet_sources)
    get_filename_component(snippet_name "${source}" NAME_WE)
    string(MAKE_C_IDENTIFIER "${snippet_name}" snippet_identifier)
    set(snippet_target "snippet_${snippet_identifier}")

    add_executable("${snippet_target}" "${source}")
    set_target_properties("${snippet_target}" PROPERTIES OUTPUT_NAME "${snippet_name}")
    target_compile_features("${snippet_target}" PRIVATE cxx_std_23)
    target_link_libraries("${snippet_target}" PRIVATE ${PROJECT_NAME}_core)

    if (ENABLE_STRICT_CHECKS)
        target_link_libraries("${snippet_target}" PRIVATE ${PROJECT_NAME}_strict_checks)
    endif (ENABLE_STRICT_CHECKS)

    if (ENABLE_SANITIZERS)
        target_link_libraries("${snippet_target}" PRIVATE ${PROJECT_NAME}_sanitizers)
    endif (ENABLE_SANITIZERS)
endforeach()
