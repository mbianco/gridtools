# This file is part of the GridTools source tree to drive installation of the examples.

function(install_example)
    set(options)
    # - if needed later split into mc and cuda sources (then add extension)
    set(one_value_args DIRECTORY)
    set(multi_value_args SOURCES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args};" "${multi_value_args}" ${ARGN})

    string(REPLACE ";" " " EXAMPLES_SRCFILES "${ARG_SOURCES}")
    configure_file(cmake_skeletons/CMakeLists.txt.example.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${ARG_DIRECTORY}/CMakeLists.txt @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${ARG_DIRECTORY}/CMakeLists.txt DESTINATION ${GT_INSTALL_EXAMPLES_PATH}/${ARG_DIRECTORY})
    install(
        DIRECTORY ${ARG_DIRECTORY}
        DESTINATION ${GT_INSTALL_EXAMPLES_PATH}
        )

    list(APPEND enabled_examples ${ARG_DIRECTORY})
    set(enabled_examples ${enabled_examples} PARENT_SCOPE)
endfunction()

if(GT_INSTALL_EXAMPLES)
    set(enabled_examples) # used in CMakeLists.txt.in, filled in install_example()
    if(COMPONENT_STENCIL_COMPOSITION)
        install_example(DIRECTORY stencil_composition SOURCES copy_stencil horizontal_diffusion_limited tridiagonal_solver)
    endif()

    if(COMPONENT_BOUNDARY_CONDITIONS)
        install_example(DIRECTORY boundary_conditions SOURCES boundary_conditions boundary_conditions_provided)
    endif()

    if(COMPONENT_C_BINDINGS)
        configure_file(c_bindings/CMakeLists.txt.in
            ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/c_bindings/CMakeLists.txt @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/c_bindings/CMakeLists.txt
            DESTINATION ${GT_INSTALL_EXAMPLES_PATH}/c_bindings)
        install(
            DIRECTORY c_bindings
            DESTINATION ${GT_INSTALL_EXAMPLES_PATH}
            PATTERN "CMakeLists.txt.in" EXCLUDE
            )
        list(APPEND enabled_examples c_bindings)
    endif()

    configure_file(cmake_skeletons/CMakeLists.txt.driver.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeLists.txt @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeLists.txt DESTINATION ${GT_INSTALL_EXAMPLES_PATH})

    install(FILES ${CMAKE_SOURCE_DIR}/cmake/workaround_check_language.cmake DESTINATION ${GT_INSTALL_EXAMPLES_PATH})
endif()
