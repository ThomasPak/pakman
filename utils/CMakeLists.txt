include_directories (${MPI_CXX_INCLUDE_DIRS})
include_directories (${PROJECT_SOURCE_DIR}/src)

add_executable (run-mpi-simulator run-mpi-simulator.cc)
target_link_libraries (run-mpi-simulator core master)

# Install targets
install (TARGETS run-mpi-simulator DESTINATION bin)
