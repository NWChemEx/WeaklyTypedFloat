# WeaklyTypedFloat
Type-erasure for floating point types. Facilitates writing general floating-point code without needing to worry about the exact floating-point type being used.

## Building

This is a pure C++ library that uses CMake as a build system.

### Requirements
- CMake 3.14 or higher
- C++17 compatible compiler

### Build Instructions

```bash
# Create a build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the library
cmake --build .

# Run tests
ctest --output-on-failure
```

### Installation

```bash
# From the build directory
cmake --install . --prefix /path/to/install
```

## Usage

```cpp
#include "weakly_typed_float/weakly_typed_float.hpp"

using namespace weakly_typed_float;

// Create from different floating point types
WeaklyTypedFloat a(3.14f);      // from float
WeaklyTypedFloat b(2.71828);    // from double
WeaklyTypedFloat c(1.41421L);   // from long double

// Perform arithmetic operations
WeaklyTypedFloat sum = a + b;
WeaklyTypedFloat product = a * b;

// Compare values
if (a < b) {
    // ...
}

// Extract as specific type
double value = sum.as<double>();
float value_f = sum.as<float>();
```

## License

Licensed under the Apache License, Version 2.0. See LICENSE file for details.
