# Get processor count
include (ProcessorCount)
ProcessorCount (NCPU)
if (NCPU EQUAL 0)
    message (WARNING "Could not find processor count, defaulting to 4")
    set (NCPU 4)
endif ()

# Add example subdirectories
add_subdirectory (standard-simulator)
add_subdirectory (mpi-simulator)
add_subdirectory (persistent-mpi-simulator)
#add_subdirectory (epithelial-cell-growth)
#add_subdirectory (biased-coin-flip)
