# Copyright 2026 NWChemEx-Project
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

"""Doc-example tests.

The tagged regions (`# [tag]` / `# [/tag]`) are literalinclude'd verbatim
into the Sphinx docs (docs/source/quickstart.rst, writing_apis.rst,
restoring_the_type.rst). Keep the tags, and the prose that references them,
in sync if you change this file.
"""

import unittest

import wtf


class TestQuickstart(unittest.TestCase):
    def test_float(self):
        # [quickstart-float-py]
        f = wtf.fp.Float(3.14)  # Python floats are always wrapped as double

        self.assertIn("3.14", f.to_string())
        self.assertEqual(f, wtf.fp.Float(3.14))

        value = f.value()  # un-erase back to a Python float
        self.assertEqual(value, 3.14)
        # [/quickstart-float-py]

    def test_buffer(self):
        # [quickstart-buffer-py]
        buffer = wtf.buffer.FloatBuffer([1.0, 2.0, 3.0])

        self.assertEqual(len(buffer), 3)
        self.assertEqual(buffer.value(), [1.0, 2.0, 3.0])
        # [/quickstart-buffer-py]


class TestRestoringTheType(unittest.TestCase):
    def test_value(self):
        # [restoring-py]
        f = wtf.fp.Float(3.14)

        value = f.value()  # or float(f); Python only ever holds a double
        self.assertEqual(value, 3.14)
        # [/restoring-py]
