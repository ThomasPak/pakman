# Configure shell scripts
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/test-abc-rejection.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-rejection.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/accept-if-epsilon-plus-parameter-is-even.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/accept-if-epsilon-plus-parameter-is-even.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/increment-and-print-number.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/increment-and-print-number.sh"
    )

# Add tests
add_test (ABCRejectionInferenceEven
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-rejection.sh" 0 10)

set_property (TEST ABCRejectionInferenceEven
    PROPERTY PASS_REGULAR_EXPRESSION "p\n2\n4\n6\n8\n10\n12\n14\n16\n18\n20\n")

add_test (ABCRejectionInferenceOdd
    "${CMAKE_CURRENT_BINARY_DIR}/test-abc-rejection.sh" 1 10)

set_property (TEST ABCRejectionInferenceOdd
    PROPERTY PASS_REGULAR_EXPRESSION "p\n1\n3\n5\n7\n9\n11\n13\n15\n17\n19\n")
