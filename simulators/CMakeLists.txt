include_directories (${MPI_CXX_INCLUDE_DIRS})
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})

add_executable (mpi_worker mpi_worker.c)
add_executable (mpi_worker_cpp mpi_worker.cc)
add_executable (persistent_mpi_worker persistent_mpi_worker.c)
add_executable (persistent_mpi_worker_cpp persistent_mpi_worker.cc)

target_link_libraries (mpi_worker ${MPI_CXX_LIBRARIES})
target_link_libraries (mpi_worker_cpp ${MPI_CXX_LIBRARIES})
target_link_libraries (persistent_mpi_worker ${MPI_CXX_LIBRARIES})
target_link_libraries (persistent_mpi_worker_cpp ${MPI_CXX_LIBRARIES})
