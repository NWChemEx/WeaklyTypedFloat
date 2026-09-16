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

#include "export_buffer.hpp"
#include <vector>
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/buffer/float_buffer.hpp>

namespace wtf::buffer {

void export_buffer_view(python_module_reference m) {
    // N.B. Python has no notion of const, so only the mutable view is exported
    using view_type = BufferView<fp::Float>;
    using size_type = view_type::size_type;
    using pfloat    = python_float_type;

    // Used to describe the held type when a non-double operation is attempted
    auto held_type = [](const view_type& self) {
        return self.size() ? self.at(0).type_info().name() :
                             std::string("empty buffer");
    };

    python_class_type<view_type>(m, "BufferView")
      .def(py::init<>())
      // N.B. the aliased buffer must outlive the view, hence the keep_alive
      .def(py::init([](FloatBuffer& buffer) { return buffer.as_view(); }),
           py::keep_alive<1, 2>())
      .def(py::init<const view_type&>(), py::keep_alive<1, 2>())
      .def("size", &view_type::size)
      .def("__len__", &view_type::size)
      .def("is_contiguous", &view_type::is_contiguous)
      // N.B. at() throws std::out_of_range, which pybind11 maps to IndexError
      .def(
        "at", [](view_type& self, size_type i) { return self.at(i); },
        py::keep_alive<0, 1>())
      .def(
        "__getitem__", [](view_type& self, size_type i) { return self.at(i); },
        py::keep_alive<0, 1>())
      .def("__setitem__",
           [=](view_type& self, size_type i, pfloat value) {
               auto element = self.at(i);
               as_python_float(
                 [&]() {
                     element = value;
                     return 0;
                 },
                 held_type(self));
           })
      .def("value",
           [=](view_type& self) {
               return as_python_float(
                 [&]() {
                     auto span = contiguous_buffer_cast<pfloat>(self);
                     return std::vector<pfloat>(span.begin(), span.end());
                 },
                 held_type(self));
           })
      .def(
        "__eq__",
        [](const view_type& self, const view_type& other) {
            return self == other;
        },
        py::is_operator())
      .def(
        "__ne__",
        [](const view_type& self, const view_type& other) {
            return self != other;
        },
        py::is_operator());
}

} // namespace wtf::buffer
