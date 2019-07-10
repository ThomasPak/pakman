include_directories (${MPI_CXX_INCLUDE_DIRS})
include_directories (${PROJECT_SOURCE_DIR}/src)

add_executable (run-mpi-worker run-mpi-worker.cc)
target_link_libraries (run-mpi-worker core master)

# Install targets
install (TARGETS run-mpi-worker DESTINATION bin)
