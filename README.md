<!--
  ~ Copyright 2025 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

# WeaklyTypedFloat (WTF)

This repo is still under heavy development! I am hoping for an alpha release
this week.

The goal of WTF is to provide a small domain-specific language (DSL) that can
be used to unify interfaces involving floating-point types. Using WTF the user
writes their interfaces in terms of "weakly typed" objects like `Float` and
`FloatBuffer`, and determination of the actual floating-point type is deferred
to runtime.

## Development Setup

### Prerequisites

- CMake >= 3.14
- A C++20-capable compiler
- Python >= 3.8

### Editable install

Create and activate a virtual environment, then install the package in editable
(dev) mode. This builds the Python bindings and the C++ test suite:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -e ".[dev]"
```

### Running the C++ tests

The editable install keeps its build tree in `build/` (configured via
`build-dir` in `pyproject.toml`). Run CTest directly from there:

```bash
cd build && ctest --output-on-failure
```

### Running the Python tests

With the virtual environment active:

```bash
pytest tests/python/
```

## Problem Description

Full description (TODO: Add link to the documentation)

Depending on the standard, C++ already natively has a lot of floating-point
types, e.g., `float`, `double`, `long double`, `std::complex<float>`, and
`std::complex<double>`. This is before considering custom floating-point types,
such as those needed for automatic differentiation, or hardware-specific types.
When attempting to maintain interfaces with explicit types (such as is required
for C-style interfaces) this leads to a combinatorial explosion in the number
of interfaces.
