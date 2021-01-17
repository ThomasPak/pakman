add_library (interface
    LineString.cc
    input.cc
    protocols.cc
    output.cc
    serialisation.cc
    deserialisation.cc
    )

target_link_libraries (interface core)

add_executable (interface_test
    unittest.cc
    )

target_link_libraries (interface_test interface)

add_test (InterfaceLibraryUnitTest
    "${CMAKE_CURRENT_BINARY_DIR}/interface_test")
