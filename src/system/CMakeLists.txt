include_directories (${PROJECT_SOURCE_DIR}/src)
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_library (system
    debug.cc
    pipe_io.cc
    system_call.cc
    signal_handler.cc
    )
