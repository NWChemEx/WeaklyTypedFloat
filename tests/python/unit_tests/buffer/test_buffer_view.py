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

"""Unit tests for the wtf.buffer.BufferView Python binding."""

import gc
import unittest

import wtf


class TestBufferView(unittest.TestCase):
    def setUp(self):
        self.values = [1.0, 2.0, 3.0]
        self.b = wtf.buffer.FloatBuffer(self.values)
        self.v = wtf.buffer.BufferView(self.b)
        self.defaulted = wtf.buffer.BufferView()

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(len(self.defaulted), 0)

    def test_buffer_ctor(self):
        self.assertEqual(len(self.v), 3)
        self.assertEqual(self.v.value(), self.values)

    def test_copy_ctor(self):
        other = wtf.buffer.BufferView(self.v)
        self.assertEqual(other, self.v)

        # The copy aliases the same buffer, so writes are visible through both
        other[0] = 9.0
        self.assertEqual(self.v[0], 9.0)
        self.assertEqual(self.b[0], 9.0)

    def test_size(self):
        self.assertEqual(self.v.size(), 3)
        self.assertEqual(len(self.v), 3)

    def test_is_contiguous(self):
        self.assertTrue(self.v.is_contiguous())

    def test_at(self):
        self.assertEqual(self.v.at(0).value(), 1.0)
        self.assertEqual(self.v.at(2).value(), 3.0)

    def test_getitem(self):
        for i, value in enumerate(self.values):
            self.assertEqual(self.v[i].value(), value)

    def test_getitem_out_of_range(self):
        with self.assertRaises(IndexError):
            self.v[3]

    def test_setitem(self):
        self.v[1] = 9.0

        # The aliased buffer sees the change too
        self.assertEqual(self.v.value(), [1.0, 9.0, 3.0])
        self.assertEqual(self.b.value(), [1.0, 9.0, 3.0])

    def test_write_through_element(self):
        element = self.v[1]
        element.set_value(9.0)
        self.assertEqual(self.b.value(), [1.0, 9.0, 3.0])

    def test_keeps_buffer_alive(self):
        v = wtf.buffer.BufferView(wtf.buffer.FloatBuffer([1.0, 2.0]))
        gc.collect()
        self.assertEqual(v.value(), [1.0, 2.0])

    def test_value(self):
        self.assertEqual(self.v.value(), self.values)
        self.assertEqual(self.defaulted.value(), [])

    def test_comparisons(self):
        self.assertEqual(self.v, wtf.buffer.BufferView(self.b))
        self.assertNotEqual(
            self.v, wtf.buffer.BufferView(wtf.buffer.FloatBuffer([1.0, 2.0]))
        )
        self.assertNotEqual(self.v, self.defaulted)
