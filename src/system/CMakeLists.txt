add_library (system
    debug.cc
    pipe_io.cc
    system_call.cc
    signal_handler.cc
    )

target_link_libraries (system core)
