# Include TestPakman
include ("../cmake/TestPakman.cmake")

# Add test subdirectories
add_subdirectory (standard-simulator)
add_subdirectory (mpi-simulator)
add_subdirectory (abc-rejection)
add_subdirectory (abc-smc)
add_subdirectory (seed)
