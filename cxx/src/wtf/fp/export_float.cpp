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

namespace wtf::fp {

void export_float(python_module_reference m) {
    using float_type = Float;
    using pfloat     = python_float_type;

    auto unwrap = [](float_type& self) {
        return as_python_float([&]() { return float_cast<pfloat>(self); },
                               self.type_info().name());
    };

    python_class_type<float_type>(m, "Float")
      .def(py::init<pfloat>())
      .def(py::init<const float_type&>())
      .def("value", unwrap)
      .def("__float__", unwrap)
      .def("to_string", &float_type::to_string)
      .def("__str__", &float_type::to_string)
      .def("__repr__",
           [](const float_type& self) {
               return "Float(" + self.to_string() + ")";
           })
      .def("type_name",
           [](const float_type& self) { return self.type_info().name(); })
      .def(
        "as_view", [](float_type& self) { return self.as_view(); },
        py::keep_alive<0, 1>())
      .def(py::self == py::self)
      .def(py::self != py::self);

    // Lets Python floats be used anywhere a Float is expected
    py::implicitly_convertible<pfloat, float_type>();
}

} // namespace wtf::fp
