
include_directories( ../../include/communication/low-level/ ) ## Library header files
include_directories( ../../include/communication/high-level ) ## Library header files

### Executables
if("${GCL_ONLY}" STREQUAL "OFF")
###   message("${HOST_SPECIFIC_OPTIONS}")
###   message("${CMAKE_CXX_FLAGS}")
###   set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HOST_SPECIFIC_OPTIONS}" )
###   message("${CMAKE_CXX_FLAGS}")
  ### L3
  if( "${GCL_MPI}" STREQUAL "ON" )
    #CPU 2D MPI tests
    foreach(srcfile Halo_Exchange_test Halo_Exchange_test_3D Halo_Exchange_test_2  proc_grid_2D_test proc_grid_3D_test)
      add_executable( ${srcfile} ../../unit_tests/communication/${srcfile}.cpp)
      add_dependencies(${srcfile} gcl)
          set_target_properties(${srcfile} PROPERTIES COMPILE_FLAGS "${HOST_SPECIFIC_OPTIONS}")
      target_link_libraries(${srcfile} gcl ${MPI_ADDITIONAL_LIBS} ${CUDA_LIBRARIES})
    endforeach(srcfile)
  endif()

  ### L2
  if( "${GCL_MPI}" STREQUAL "ON" )
    #CPU 2D MPI tests
    foreach(srcfile test_descriptors test_descriptors_2D test_descriptors_3D test_empty_descriptors_2D test_empty_descriptors_3D boolist ndloops layout_map)
      add_executable( ${srcfile} ../../unit_tests/communication/${srcfile}.cpp)
      add_dependencies(${srcfile} gcl)
      set_target_properties(${srcfile} PROPERTIES COMPILE_FLAGS "${HOST_SPECIFIC_OPTIONS}")
      target_link_libraries(${srcfile} gcl ${MPI_ADDITIONAL_LIBS} ${CUDA_LIBRARIES})
    endforeach(srcfile)
  endif()
endif()
