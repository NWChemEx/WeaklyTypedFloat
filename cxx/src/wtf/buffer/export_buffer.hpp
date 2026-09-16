/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "../export_wtf.hpp"

namespace wtf::buffer {

/** @brief Exports the wtf::buffer::FloatBuffer class to the module @p m. */
void export_float_buffer(python_module_reference m);

/** @brief Exports the wtf::buffer::BufferView class to the module @p m. */
void export_buffer_view(python_module_reference m);

/** @brief Exports the contents of the wtf::buffer namespace to @p m.
 *
 *  This function creates the "buffer" submodule of @p m and then exports each
 *  class in the wtf::buffer namespace into it. Classes are exported
 *  hierarchically, i.e., a class is exported after any class it depends on.
 *
 *  @param[in,out] m The module to add the "buffer" submodule to.
 */
inline void export_buffer(python_module_reference m) {
    auto mbuffer = m.def_submodule("buffer");
    export_float_buffer(mbuffer);
    export_buffer_view(mbuffer);
}

} // namespace wtf::buffer
