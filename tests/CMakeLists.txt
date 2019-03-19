# Include TestPakman
include (TestPakman.cmake)

# Add test subdirectories
add_subdirectory (standard-simulator)
add_subdirectory (mpi-simulator)
add_subdirectory (persistent-mpi-simulator)
add_subdirectory (abc-rejection)
add_subdirectory (abc-smc)
