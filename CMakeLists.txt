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
project(wtf VERSION "1.0.0" LANGUAGES CXX)
include(cmake/disable_in_source_builds.cmake)

set(CMAKE_CXX_STANDARD 20)
# TODO: Make these into proper options
set(BUILD_TESTING TRUE)
set(CMAKE_BUILD_SHARED_LIBS ON)


file(GLOB_RECURSE WTF_HEADER_FILES CONFIGURE_DEPENDS include/wtf/*.hpp)
file(GLOB_RECURSE WTF_SOURCE_FILES CONFIGURE_DEPENDS src/wtf/*.cpp)
add_library(${PROJECT_NAME} ${WTF_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
if(${BUILD_TESTING})
    include(CTest)
    include(cmake/catch2_tests_from_dir.cmake)

    find_catch2("v3.11.0")
    catch2_tests_from_dir(
        "unit_test_${PROJECT_NAME}" 
        tests/unit_tests/wtf 
        ${PROJECT_NAME}
    )
endif()