# Copyright 2025 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.14)

# Downloads common CMake modules used throughout NWChemEx. Included before
# project() so nwx_set_version() is available to compute the version.
include(cmake/get_nwx_cmake.cmake)

# Version from scikit-build-core (wheels/sdists) or the latest git tag.
include(nwx_set_version)
nwx_set_version(wtf_version "${CMAKE_CURRENT_LIST_DIR}")
project(wtf VERSION "${wtf_version}" LANGUAGES CXX)

include(disable_in_source_builds)
include(set_default_nwx_options)

# Define the documentation target
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("cxx/include" "cxx/src")

# Build the Library
include(nwx_library)
nwx_library(${PROJECT_NAME} "cxx/include" "cxx/src")

# Register the C++ tests
include(catch2_tests_from_dir)
catch2_tests_from_dir(
    "unit_test_${PROJECT_NAME}"
    "tests/cxx/unit_tests/${PROJECT_NAME}"
    ${PROJECT_NAME}
)

# Register the Python module
include(nwx_python_module)
nwx_python_module(${PROJECT_NAME} "cxx/src")
