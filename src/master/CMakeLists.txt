include_directories (${MPI_CXX_INCLUDE_DIRS})
add_compile_options (${MPI_CXX_COMPILE_OPTIONS})

add_library (master
    AbstractMaster.cc
    AbstractMasterStatic.cc
    SerialMaster.cc
    SerialMasterStatic.cc
    MPIMaster.cc
    MPIMasterStatic.cc
    Manager.cc
    AbstractWorkerHandler.cc
    ForkedWorkerHandler.cc
    MPIWorkerHandler.cc
    )

target_link_libraries (master core system mpi controller ${MPI_CXX_LIBRARIES})
