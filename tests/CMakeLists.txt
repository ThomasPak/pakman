# If hosts flags are given, add them to MPIEXEC_PREFLAGS
if (MPIEXEC_HOSTS_FLAGS)
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} ${MPIEXEC_HOSTS_FLAGS}")
    message ("Host flags are: ${MPIEXEC_HOSTS_FLAGS}")
endif ()

# If the MPI installation is OpenMPI, make sure to add `--oversubscribe` flags
# to MPIEXEC_PREFLAGS
execute_process (COMMAND ${MPIEXEC_EXECUTABLE} --version
    OUTPUT_VARIABLE mpiexec_version)

if (mpiexec_version MATCHES "OpenRTE")
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} --oversubscribe")
endif ()

# Separate MPIEXEC_PREFLAGS and MPIEXEC_POSTFLAGS
separate_arguments (MPIEXEC_PREFLAGS)
separate_arguments (MPIEXEC_POSTFLAGS)

# Include TestPakman
include (TestPakman.cmake)

# Add test subdirectories
add_subdirectory (standard-simulator)
add_subdirectory (mpi-simulator)
add_subdirectory (persistent-mpi-simulator)
