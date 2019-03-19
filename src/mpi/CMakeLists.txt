include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

include_directories (${MPI_CXX_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})
link_libraries (${MPI_CXX_LIBRARIES} pakman_base)

add_library (pakman_mpi_base
    MPIMaster.cc
    MPIMasterStatic.cc
    Manager.cc
    AbstractWorkerHandler.cc
    ForkedWorkerHandler.cc
    MPIWorkerHandler.cc
    PersistentMPIWorkerHandler.cc
    mpi_utils.cc
    spawn.cc
    )
