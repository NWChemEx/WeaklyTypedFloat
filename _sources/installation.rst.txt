.. Copyright 2026 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

############
Installation
############

.. note::

   WTF is still under heavy development and has not had a stable release
   yet. For now, building from source (as described on this page) is the
   only supported way to get it.

*************
Prerequisites
*************

- CMake >= 3.14
- A C++20-capable compiler
- Python >= 3.8

********************************
Installing for development (pip)
********************************

The easiest way to get both the Python bindings and the C++ test suite built
is an editable pip install. Create and activate a virtual environment, then
install WTF in editable (dev) mode:

.. code-block:: bash

   python3 -m venv .venv
   source .venv/bin/activate
   pip install -e ".[dev]"

This passes ``-DDEVELOPER_SETUP=ON`` to CMake under the hood (see
``pyproject.toml``), and keeps its build tree in ``build/``.

****************************
Building with CMake directly
****************************

If you would rather configure and build WTF yourself, without going through
pip, the equivalent steps are:

.. code-block:: bash

   cmake -S . -B build -DDEVELOPER_SETUP=ON
   cmake --build build
   cd build && ctest --output-on-failure

Configuring pulls in the shared NWChemEx CMake modules automatically (from a
local pip-installed ``nwxcmake`` package if one is found, otherwise by
fetching them from GitHub), so no manual module setup is required beyond the
prerequisites above.

*********************
Running the C++ tests
*********************

From the build tree created by either method above:

.. code-block:: bash

   cd build && ctest --output-on-failure

************************
Running the Python tests
************************

With your virtual environment active:

.. code-block:: bash

   pytest tests/python/

**********
Next steps
**********

Head to :doc:`quickstart` for a first look at WTF's API.
