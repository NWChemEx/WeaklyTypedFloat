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

"""Unit tests for the wtf.fp.Float Python binding."""

import gc
import unittest

import wtf


class TestFloat(unittest.TestCase):
    def setUp(self):
        self.value = 3.14
        self.f = wtf.fp.Float(self.value)

    def test_ctor(self):
        self.assertEqual(self.f.value(), self.value)

    def test_copy_ctor(self):
        other = wtf.fp.Float(self.f)
        self.assertEqual(other, self.f)
        self.assertEqual(other.value(), self.value)

    def test_value(self):
        self.assertEqual(self.f.value(), self.value)
        self.assertEqual(float(self.f), self.value)

    def test_to_string(self):
        self.assertIn("3.14", self.f.to_string())
        self.assertEqual(str(self.f), self.f.to_string())
        self.assertEqual(repr(self.f), "Float(" + self.f.to_string() + ")")

    def test_type_name(self):
        # Python floats are always wrapped as doubles
        self.assertEqual(self.f.type_name(), "double")

    def test_as_view(self):
        v = self.f.as_view()
        self.assertEqual(v.value(), self.value)

        # Writing through the view changes the Float
        v.set_value(1.23)
        self.assertEqual(self.f.value(), 1.23)

    def test_as_view_keeps_float_alive(self):
        v = wtf.fp.Float(1.23).as_view()
        gc.collect()
        self.assertEqual(v.value(), 1.23)

    def test_comparisons(self):
        self.assertEqual(self.f, wtf.fp.Float(self.value))
        self.assertNotEqual(self.f, wtf.fp.Float(1.23))

        # Python floats implicitly convert to Float
        self.assertEqual(self.f, self.value)
        self.assertNotEqual(self.f, 1.23)

    def test_comparison_with_other_types(self):
        # Comparing against an unrelated type is False, not an error
        self.assertNotEqual(self.f, "3.14")
        self.assertNotEqual(self.f, None)
