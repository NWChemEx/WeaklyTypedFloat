# Simple test executable without external dependencies
add_executable(test_weakly_typed_float
    test_weakly_typed_float.cpp
)

target_link_libraries(test_weakly_typed_float
    PRIVATE weakly_typed_float
)

# Add test
add_test(NAME test_weakly_typed_float COMMAND test_weakly_typed_float)
