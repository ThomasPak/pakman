include_directories (${MPI_CXX_INCLUDE_DIRS})
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})

add_library (mpi
    mpi_utils.cc
    spawn.cc
    )

target_link_libraries (mpi core ${MPI_CXX_LIBRARIES})
