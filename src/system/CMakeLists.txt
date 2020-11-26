add_library (system
    debug.cc
    pipe_io.cc
    system_call.cc
    signal_handler.cc
    )

target_link_libraries (system core)

add_executable (system_test
    unittest.cc
    )

target_link_libraries (system_test system)

add_test (SystemLibraryUnitTest
    "${CMAKE_CURRENT_BINARY_DIR}/system_test")
