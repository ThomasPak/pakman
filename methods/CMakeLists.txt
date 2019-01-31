# Find MPI
include (FindMPI)

if (NOT MPI_MPICXX_FOUND)
    message (FATAL_ERROR "MPI installation with C++ bindings was not found")
endif (NOT MPI_MPICXX_FOUND)

add_subdirectory (mpi)

add_library (pakman_base
    ABCRejectionController.cc
    ABCSMCController.cc
    AbstractController.cc
    AbstractMaster.cc
    acceptance_probability.cc
    debug.cc
    parse_cmd.cc
    pipe_io.cc
    propose_parameter.cc
    read_input.cc
    run_simulation.cc
    sample_population.cc
    Sampler.cc
    smc_weight.cc
    SweepController.cc
    system_call.cc
    timer.cc
    vector_argv.cc
    vector_delimited.cc
    vector_printf.cc
    vector_strtok.cc
    write_parameters.cc
    c_argv.cc
    )

add_executable (rejection rejection.cc)
target_link_libraries (rejection pakman_base)

add_executable (smc smc.cc)
target_link_libraries (smc pakman_base)

add_executable (sweep sweep.cc)
target_link_libraries (sweep pakman_base)

include_directories (${MPI_CXX_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (rejection-mpi mpi/rejection-mpi.cc)
target_link_libraries (rejection-mpi pakman_base pakman_mpi_base)

add_executable (smc-mpi mpi/smc-mpi.cc)
target_link_libraries (smc-mpi pakman_base pakman_mpi_base)

add_executable (sweep-mpi mpi/sweep-mpi.cc)
target_link_libraries (sweep-mpi pakman_base pakman_mpi_base)

# Install targets
install (TARGETS rejection smc sweep rejection-mpi smc-mpi sweep-mpi
    DESTINATION bin)
install (PROGRAMS pakman DESTINATION bin)
