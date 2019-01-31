include_directories (${MPI_CXX_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/methods)
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})
link_libraries (${MPI_CXX_LIBRARIES} pakman_base)

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
