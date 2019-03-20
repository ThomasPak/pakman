# spdlog
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_subdirectory (core)
add_subdirectory (interface)
add_subdirectory (mpi)
add_subdirectory (controller)

add_library (pakman_base
    AbstractMaster.cc
    AbstractMasterStatic.cc
    SerialMaster.cc
    SerialMasterStatic.cc
    help.cc
    debug.cc
    pipe_io.cc
    system_call.cc
    signal_handler.cc
    )

target_include_directories (pakman_base PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mpi)
target_link_libraries (pakman_base pakman_mpi_base)

include_directories (${MPI_CXX_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (pakman main.cc)
target_link_libraries (pakman pakman_base interface core controller)

# Install targets
install (TARGETS pakman DESTINATION bin)
