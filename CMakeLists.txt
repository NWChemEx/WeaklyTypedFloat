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
# Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

project(wtf VERSION "1.0.0" LANGUAGES CXX)
include(cmake/disable_in_source_builds.cmake)
set(CMAKE_CXX_STANDARD 20)

# TODO: Make these into proper options
set(BUILD_TESTING TRUE)
set(CMAKE_BUILD_SHARED_LIBS ON)

include(cmake/get_dependencies.cmake)

# Define the documentation target
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("include" "src")

# Build the Library
file(GLOB_RECURSE WTF_HEADER_FILES CONFIGURE_DEPENDS include/wtf/*.hpp)
file(GLOB_RECURSE WTF_SOURCE_FILES CONFIGURE_DEPENDS src/wtf/*.cpp)
add_library(${PROJECT_NAME} ${WTF_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(${BUILD_TESTING})
    include(CTest)
    get_dependencies(catch2)
    include(cmake/catch2_tests_from_dir.cmake)
    catch2_tests_from_dir(
        "unit_test_${PROJECT_NAME}"
        tests/unit_tests/wtf
        ${PROJECT_NAME}
    )
endif()

include(cmake/install_target.cmake)
install_library(
    ${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}"
)
