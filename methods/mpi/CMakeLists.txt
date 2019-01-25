# Find MPI
include (FindMPI)

if (NOT MPI_MPICXX_FOUND)
    message (FATAL_ERROR "MPI installation with C++ bindings was not found")
endif (NOT MPI_MPICXX_FOUND)

include_directories (${MPI_CXX_INCLUDE_DIRS} ..)
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})

add_library (pakman_mpi_base
    AbstractWorkerHandler.cc
    ForkedWorkerHandler.cc
    main_utils.cc
    Manager.cc
    master.cc
    MPIMaster.cc
    mpi_utils.cc
    MPIWorkerHandler.cc
    PersistentMPIWorkerHandler.cc
    spawn.cc
    )

target_link_libraries (pakman_mpi_base ${MPI_CXX_LIBRARIES})
