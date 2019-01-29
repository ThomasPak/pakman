# Copy include file to build tree
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/pakman_mpi_worker.h"
    "${CMAKE_CURRENT_BINARY_DIR}/pakman_mpi_worker.h"
    COPYONLY)

# Install include files
install (FILES pakman_mpi_worker.h DESTINATION include)
