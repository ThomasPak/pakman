# spdlog
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (core)
add_subdirectory (interface)
add_subdirectory (system)
add_subdirectory (mpi)
add_subdirectory (controller)
add_subdirectory (master)

add_executable (pakman main/main.cc main/help.cc)
target_link_libraries (pakman core controller master)

# Install targets
install (TARGETS pakman DESTINATION bin)
