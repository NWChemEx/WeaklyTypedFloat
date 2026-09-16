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
#include <wtf/buffer/float_buffer.hpp>

namespace wtf::buffer {

void export_float_buffer(python_module_reference m) {
    using buffer_type = FloatBuffer;
    using size_type   = buffer_type::size_type;
    using value_type  = buffer_type::value_type;
    using pfloat      = python_float_type;

    // Used to describe the held type when a non-double operation is attempted
    auto held_type = [](const buffer_type& self) {
        return self.size() ? self.at(0).type_info().name() :
                             std::string("empty buffer");
    };

    python_class_type<buffer_type>(m, "FloatBuffer")
      .def(py::init<>())
      .def(py::init<const buffer_type&>())
      // N.B. registered after the copy ctor so a FloatBuffer is never
      //      mistaken for a sequence of floats
      .def(py::init([](std::vector<pfloat> values) {
          return buffer_type(std::move(values));
      }))
      .def("size", &buffer_type::size)
      .def("__len__", &buffer_type::size)
      .def("is_contiguous", &buffer_type::is_contiguous)
      // N.B. at() throws std::out_of_range, which pybind11 maps to IndexError
      .def(
        "at", [](buffer_type& self, size_type i) { return self.at(i); },
        py::keep_alive<0, 1>())
      .def(
        "__getitem__",
        [](buffer_type& self, size_type i) { return self.at(i); },
        py::keep_alive<0, 1>())
      .def("__setitem__",
           [=](buffer_type& self, size_type i, pfloat value) {
               auto element = self.at(i);
               as_python_float(
                 [&]() {
                     element = value;
                     return 0;
                 },
                 held_type(self));
           })
      .def("push_back",
           [](buffer_type& self, pfloat value) { self.push_back(value); })
      .def("push_back",
           [](buffer_type& self, value_type& value) { self.push_back(value); })
      .def("reserve",
           [](buffer_type& self, size_type n) { self.reserve<pfloat>(n); })
      .def("value",
           [=](buffer_type& self) {
               return as_python_float(
                 [&]() {
                     auto span = contiguous_buffer_cast<pfloat>(self);
                     return std::vector<pfloat>(span.begin(), span.end());
                 },
                 held_type(self));
           })
      .def(
        "as_view", [](buffer_type& self) { return self.as_view(); },
        py::keep_alive<0, 1>())
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace wtf::buffer
