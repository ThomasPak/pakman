# Find MPI
include (FindMPI)

# If the MPI installation is OpenMPI, make sure to add --oversubscribe flags
execute_process (COMMAND ${MPIEXEC_EXECUTABLE} --version
    OUTPUT_VARIABLE mpiexec_version)

if (mpiexec_version MATCHES "OpenRTE")
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} --oversubscribe")

    # Strip leading and trailing spaces, otherwise mpiexec might mistake
    # --oversubscribe for an executable
    string (STRIP "${MPIEXEC_PREFLAGS}" MPIEXEC_PREFLAGS)
endif ()

# Separate MPIEXEC_PREFLAGS and MPIEXEC_POSTFLAGS
separate_arguments (MPIEXEC_PREFLAGS)
separate_arguments (MPIEXEC_POSTFLAGS)

# Add example subdirectories
add_subdirectory (standard-simulator)
add_subdirectory (mpi-simulator)
add_subdirectory (persistent-mpi-simulator)
#add_subdirectory (epithelial-cell-growth)
#add_subdirectory (biased-coin-flip)
