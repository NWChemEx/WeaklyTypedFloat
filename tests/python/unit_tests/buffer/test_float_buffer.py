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

"""Unit tests for the wtf.buffer.FloatBuffer Python binding."""

import gc
import unittest

import wtf


class TestFloatBuffer(unittest.TestCase):
    def setUp(self):
        self.values = [1.0, 2.0, 3.0]
        self.b = wtf.buffer.FloatBuffer(self.values)
        self.defaulted = wtf.buffer.FloatBuffer()

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(len(self.defaulted), 0)

    def test_list_ctor(self):
        self.assertEqual(len(self.b), 3)
        self.assertEqual(self.b.value(), self.values)

    def test_copy_ctor(self):
        other = wtf.buffer.FloatBuffer(self.b)
        self.assertEqual(other, self.b)

        # The copy is deep, so writing to it does not change the original
        other[0] = 9.0
        self.assertEqual(other[0], 9.0)
        self.assertEqual(self.b[0], 1.0)

    def test_size(self):
        self.assertEqual(self.b.size(), 3)
        self.assertEqual(len(self.b), 3)

    def test_is_contiguous(self):
        self.assertTrue(self.b.is_contiguous())

    def test_at(self):
        self.assertEqual(self.b.at(0).value(), 1.0)
        self.assertEqual(self.b.at(2).value(), 3.0)

    def test_getitem(self):
        for i, value in enumerate(self.values):
            self.assertEqual(self.b[i].value(), value)

    def test_getitem_out_of_range(self):
        with self.assertRaises(IndexError):
            self.b[3]

    def test_setitem(self):
        self.b[1] = 9.0
        self.assertEqual(self.b.value(), [1.0, 9.0, 3.0])

    def test_setitem_out_of_range(self):
        with self.assertRaises(IndexError):
            self.b[3] = 9.0

    def test_write_through_element(self):
        element = self.b[1]
        element.set_value(9.0)
        self.assertEqual(self.b.value(), [1.0, 9.0, 3.0])

    def test_element_keeps_buffer_alive(self):
        element = wtf.buffer.FloatBuffer([1.0, 2.0])[1]
        gc.collect()
        self.assertEqual(element.value(), 2.0)

    def test_push_back_float(self):
        self.b.push_back(4.0)
        self.assertEqual(self.b.value(), [1.0, 2.0, 3.0, 4.0])

    def test_push_back_wtf_float(self):
        self.b.push_back(wtf.fp.Float(4.0))
        self.assertEqual(self.b.value(), [1.0, 2.0, 3.0, 4.0])

    def test_push_back_onto_default(self):
        self.defaulted.push_back(1.0)
        self.assertEqual(self.defaulted.value(), [1.0])

    def test_reserve(self):
        self.b.reserve(100)
        self.assertEqual(self.b.value(), self.values)

    def test_value(self):
        self.assertEqual(self.b.value(), self.values)
        self.assertEqual(self.defaulted.value(), [])

    def test_as_view(self):
        v = self.b.as_view()
        self.assertEqual(len(v), 3)

        # Writing through the view changes the buffer
        v[0] = 9.0
        self.assertEqual(self.b.value(), [9.0, 2.0, 3.0])

    def test_comparisons(self):
        self.assertEqual(self.b, wtf.buffer.FloatBuffer(self.values))
        self.assertNotEqual(self.b, wtf.buffer.FloatBuffer([1.0, 2.0]))
        self.assertNotEqual(self.b, self.defaulted)
