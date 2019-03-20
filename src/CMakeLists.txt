# spdlog
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_subdirectory (mpi)
add_subdirectory (interface)

add_library (pakman_base
    AbstractController.cc
    AbstractControllerStatic.cc
    SweepController.cc
    SweepControllerStatic.cc
    ABCRejectionController.cc
    ABCRejectionControllerStatic.cc
    ABCSMCController.cc
    ABCSMCControllerStatic.cc
    AbstractMaster.cc
    AbstractMasterStatic.cc
    SerialMaster.cc
    SerialMasterStatic.cc
    Arguments.cc
    LongOptions.cc
    Parameter.cc
    help.cc
    debug.cc
    parse_cmd.cc
    pipe_io.cc
    read_input.cc
    sample_population.cc
    Sampler.cc
    smc_weight.cc
    system_call.cc
    timer.cc
    vector_argv.cc
    vector_delimited.cc
    vector_printf.cc
    vector_strtok.cc
    write_parameters.cc
    c_argv.cc
    signal_handler.cc
    )

target_include_directories (pakman_base PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mpi)
target_link_libraries (pakman_base pakman_mpi_base)

include_directories (${MPI_CXX_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (pakman main.cc)
target_link_libraries (pakman pakman_base interface)

# Install targets
install (TARGETS pakman DESTINATION bin)
