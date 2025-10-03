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

include_guard()

function(find_catch2 fc_version)
    include(FetchContent)
    FetchContent_Declare(
        catch2 
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG "${fc_version}"
    )
    FetchContent_MakeAvailable(catch2)
endfunction()

function(catch2_tests_from_dir ctfd_target_name ctfd_dir)
    file(GLOB_RECURSE ctfd_test_files CONFIGURE_DEPENDS ${ctfd_dir}/*.cpp)
    
    add_executable(${ctfd_target_name} ${ctfd_test_files})
    
    target_link_libraries(
        ${ctfd_target_name} PRIVATE Catch2::Catch2WithMain ${ARGN})
    
        add_test(NAME ${ctfd_target_name}
             COMMAND ${ctfd_target_name}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()