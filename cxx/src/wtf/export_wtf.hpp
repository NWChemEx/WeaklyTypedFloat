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
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>

namespace wtf {

namespace py = pybind11;

/// Type of a reference to the Python module the bindings are added to
using python_module_reference = py::module_&;

/// Type of a Python class being exported
template<typename... Args>
using python_class_type = py::class_<Args...>;

/** @brief The floating-point type Python is limited to.
 *
 *  Python's native `float` is a C++ `double`, which is the only floating-point
 *  type that crosses the Python/C++ boundary.
 */
using python_float_type = double;

/** @brief Runs @p fxn, mapping a WTF bad-cast into a Python TypeError.
 *
 *  Every binding which unwraps (or overwrites) a type-erased value does so as
 *  a python_float_type. When the type-erased object is holding some other
 *  floating-point type the underlying WTF call throws a std::runtime_error.
 *  By default pybind11 surfaces that as a Python RuntimeError, which does not
 *  tell the user what went wrong. This function instead raises a TypeError
 *  naming the type that is actually being held.
 *
 *  @tparam Fxn The type of the callable to run. Will be deduced.
 *
 *  @param[in] fxn The callable performing the double-typed operation.
 *  @param[in] held_type The name of the type the WTF object is holding. Used
 *                       to make the error message actionable.
 *
 *  @return Whatever @p fxn returns.
 *
 *  @throw pybind11::type_error if @p fxn throws std::runtime_error. Strong
 *                              throw guarantee.
 */
template<typename Fxn>
auto as_python_float(Fxn&& fxn, const std::string& held_type) {
    try {
        return fxn();
    } catch(const std::runtime_error&) {
        throw py::type_error("This object holds a " + held_type +
                             ", which Python can not represent. Only double is "
                             "supported at the Python boundary.");
    }
}

} // namespace wtf
