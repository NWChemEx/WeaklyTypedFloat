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

namespace wtf::fp {

/** @brief Exports the wtf::fp::Float class to the Python module @p m. */
void export_float(python_module_reference m);

/** @brief Exports the wtf::fp::FloatView class to the Python module @p m. */
void export_float_view(python_module_reference m);

/** @brief Exports the contents of the wtf::fp namespace to @p m.
 *
 *  This function creates the "fp" submodule of @p m and then exports each
 *  class in the wtf::fp namespace into it. Classes are exported
 *  hierarchically, i.e., a class is exported after any class it depends on.
 *
 *  @param[in,out] m The module to add the "fp" submodule to.
 */
inline void export_fp(python_module_reference m) {
    auto mfp = m.def_submodule("fp");
    export_float(mfp);
    export_float_view(mfp);
}

} // namespace wtf::fp
