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

"""Unit tests for the wtf.fp.FloatView Python binding."""

import gc
import unittest

import wtf


class TestFloatView(unittest.TestCase):
    def setUp(self):
        self.value = 3.14
        self.f = wtf.fp.Float(self.value)
        self.v = wtf.fp.FloatView(self.f)

    def test_ctor(self):
        self.assertEqual(self.v.value(), self.value)

    def test_copy_ctor(self):
        other = wtf.fp.FloatView(self.v)
        self.assertEqual(other, self.v)

        # The copy aliases the same value, so writing through it is visible
        other.set_value(1.23)
        self.assertEqual(self.v.value(), 1.23)
        self.assertEqual(self.f.value(), 1.23)

    def test_value(self):
        self.assertEqual(self.v.value(), self.value)
        self.assertEqual(float(self.v), self.value)

    def test_set_value(self):
        self.v.set_value(1.23)
        self.assertEqual(self.v.value(), 1.23)

        # The aliased Float sees the change too
        self.assertEqual(self.f.value(), 1.23)

    def test_to_string(self):
        self.assertIn("3.14", self.v.to_string())
        self.assertEqual(str(self.v), self.v.to_string())
        self.assertEqual(repr(self.v), "FloatView(" + self.v.to_string() + ")")

    def test_type_name(self):
        self.assertEqual(self.v.type_name(), "double")

    def test_keeps_float_alive(self):
        v = wtf.fp.FloatView(wtf.fp.Float(1.23))
        gc.collect()
        self.assertEqual(v.value(), 1.23)

    def test_comparisons(self):
        self.assertEqual(self.v, wtf.fp.FloatView(self.f))
        self.assertEqual(self.v, self.f)
        self.assertEqual(self.v, self.value)

        other = wtf.fp.Float(1.23)
        self.assertNotEqual(self.v, wtf.fp.FloatView(other))
        self.assertNotEqual(self.v, other)
        self.assertNotEqual(self.v, 1.23)

    def test_comparison_with_other_types(self):
        self.assertNotEqual(self.v, "3.14")
        self.assertNotEqual(self.v, None)
