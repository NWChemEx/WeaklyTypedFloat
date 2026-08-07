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

"""Unit tests for the wtf.Float Python binding."""

import wtf


def test_import():
    assert hasattr(wtf, "Float")


def test_set_value():
    f = wtf.Float(3.14)
    assert f is not None


def test_to_string():
    f = wtf.Float(3.14)
    s = f.to_string()
    assert "3.14" in s
