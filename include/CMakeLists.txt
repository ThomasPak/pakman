# Copy include files to build tree
file (COPY pakman_mpi_worker.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY PakmanMPIWorker.hpp
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install include files
install (FILES pakman_mpi_worker.h DESTINATION include)
install (FILES PakmanMPIWorker.hpp DESTINATION include)
