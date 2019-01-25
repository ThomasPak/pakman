# Find MPI
include (FindMPI)

if (NOT MPI_MPICXX_FOUND)
    message (FATAL_ERROR "MPI installation with C++ bindings was not found")
endif (NOT MPI_MPICXX_FOUND)

include_directories (${MPI_CXX_INCLUDE_DIRS})
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})

add_executable (mpi_worker mpi_worker.c)
add_executable (persistent_mpi_worker persistent_mpi_worker.c)

target_link_libraries (mpi_worker ${MPI_CXX_LIBRARIES})
target_link_libraries (persistent_mpi_worker ${MPI_CXX_LIBRARIES})
