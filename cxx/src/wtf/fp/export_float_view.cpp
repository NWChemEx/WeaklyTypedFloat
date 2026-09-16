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

#include "export_fp.hpp"
#include <wtf/fp/float.hpp>
#include <wtf/fp/float_view.hpp>

namespace wtf::fp {

void export_float_view(python_module_reference m) {
    // N.B. Python has no notion of const, so only the mutable view is exported
    using view_type = FloatView<Float>;
    using pfloat    = python_float_type;

    auto unwrap = [](view_type& self) {
        return as_python_float([&]() { return self.value<pfloat>(); },
                               self.type_info().name());
    };

    python_class_type<view_type>(m, "FloatView")
      // N.B. the aliased object must outlive the view, hence the keep_alive
      .def(py::init<Float&>(), py::keep_alive<1, 2>())
      .def(py::init<const view_type&>(), py::keep_alive<1, 2>())
      .def("value", unwrap)
      .def("__float__", unwrap)
      .def("set_value",
           [](view_type& self, pfloat value) {
               as_python_float(
                 [&]() {
                     self = value;
                     return 0;
                 },
                 self.type_info().name());
           })
      .def("to_string", &view_type::to_string)
      .def("__str__", &view_type::to_string)
      .def("__repr__",
           [](const view_type& self) {
               return "FloatView(" + self.to_string() + ")";
           })
      .def("type_name",
           [](const view_type& self) { return self.type_info().name(); })
      .def(
        "__eq__",
        [](const view_type& self, const view_type& other) {
            return self == other;
        },
        py::is_operator())
      .def(
        "__eq__",
        [](const view_type& self, Float& other) {
            return self == other.as_view();
        },
        py::is_operator())
      .def(
        "__eq__",
        [](const view_type& self, pfloat other) { return self == other; },
        py::is_operator())
      .def(
        "__ne__",
        [](const view_type& self, const view_type& other) {
            return self != other;
        },
        py::is_operator())
      .def(
        "__ne__",
        [](const view_type& self, Float& other) {
            return self != other.as_view();
        },
        py::is_operator())
      .def(
        "__ne__",
        [](const view_type& self, pfloat other) { return !(self == other); },
        py::is_operator());
}

} // namespace wtf::fp
