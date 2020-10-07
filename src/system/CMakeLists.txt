add_library (system
    debug.cc
    pipe_io.cc
    system_call.cc
    signal_handler.cc
    )

target_link_libraries (system core)

add_executable (unittest
    unittest.cc
    )

target_link_libraries (unittest system)

add_test (SystemLibraryUnitTest
    "${CMAKE_CURRENT_BINARY_DIR}/unittest")
