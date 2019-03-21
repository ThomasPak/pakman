# Configure shell scripts
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/test-abc-smc.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-smc.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/perturber.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/perturber.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/prior-pdf.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/prior-pdf.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/perturbation-pdf.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/perturbation-pdf.sh"
    )

# Add tests
add_test (ABCSMCInferenceEven
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-smc.sh" 2,1,0 10)

add_test (ABCSMCInferenceOdd
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-smc.sh" 3,2,1 10)
