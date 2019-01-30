# Copy include file to build tree
file (COPY pakman_mpi_worker.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install include files
install (FILES pakman_mpi_worker.h DESTINATION include)
