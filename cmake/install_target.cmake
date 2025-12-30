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

function(write_config_file wcf_file)
    file(WRITE "${wcf_file}" "") # Erases it if it already exists
    file(APPEND
        "${wcf_file}"
        "get_filename_component(_IL_CONFIG_DIR "
        "\"\${CMAKE_CURRENT_LIST_FILE}\" PATH)\n"
    )
    file(APPEND
        "${wcf_file}"
        "include(\"\${_IL_CONFIG_DIR}/${il_name}Targets.cmake\")\n"
    )
endfunction()

function(install_library il_name il_header_dir)
    #TODO: Get these values programmatically
    set(_il_archive_dir lib)
    set(_il_library_dir lib)
    set(_il_runtime_dir bin)
    set(_il_includes_dir include)

    # -- Install target that is a library --
    install(TARGETS ${il_name}
        EXPORT ${il_name}Targets
        ARCHIVE DESTINATION "${_il_archive_dir}"
        LIBRARY DESTINATION "${_il_library_dir}"
        RUNTIME DESTINATION "${_il_runtime_dir}"
        INCLUDES DESTINATION "${_il_includes_dir}"
    )

    # -- Install CMake Config Files --
    install(EXPORT ${il_name}Targets
        FILE ${il_name}Targets.cmake
        NAMESPACE ${il_name}::
        DESTINATION "${_il_library_dir}/cmake/${il_name}"
    )

    set(_il_config_file "${CMAKE_CURRENT_BINARY_DIR}/${il_name}Config.cmake")
    write_config_file("${_il_config_file}")

    install(FILES "${_il_config_file}"
        DESTINATION "${_il_library_dir}/cmake/${il_name}"
    )

    # -- Install Headers --
    #TODO: Assert il_header_dir isn't empty
    install(DIRECTORY "${il_header_dir}"
        DESTINATION "${_il_includes_dir}"
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.h"
    )
endfunction()
