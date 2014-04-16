## # add_executable(interface1_notemps_naive   interface1_notemps.cpp ${SOURCES} )
## # add_executable(interface1_notemps_block   interface1_notemps.cpp ${SOURCES} )
## # set_target_properties(interface1_notemps_block
## #         PROPERTIES COMPILE_DEFINITIONS "BACKEND_BLOCK")

#add_executable(interface2   interface2.cpp ${SOURCES} )

add_executable(interface1_naive   interface1.cpp )
add_executable(interface1_block   interface1.cpp )
set_target_properties(interface1_block
        PROPERTIES COMPILE_DEFINITIONS "BACKEND_BLOCK")

add_executable(laplacian_naive   laplacian.cpp )
add_executable(laplacian_block   laplacian.cpp )
set_target_properties(laplacian_block
        PROPERTIES COMPILE_DEFINITIONS "BACKEND_BLOCK")

if(CUDA_FOUND)
    cuda_add_executable( interface1_cuda interface1.cu )
    #add_dependencies(  )
    #target_link_libraries( interface1_cuda ${CUDA_MPI_LIB} )

    cuda_add_executable( laplacian_cuda laplacian.cu )
endif()