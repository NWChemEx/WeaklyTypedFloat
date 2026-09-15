.. Copyright 2025 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

###############
Python Bindings
###############

WTF ships a Python module, ``wtf``, whose submodules mirror the C++
namespaces. Four classes are currently exposed:

============================  =========================
C++                           Python
============================  =========================
``wtf::fp::Float``            ``wtf.fp.Float``
``wtf::fp::FloatView``        ``wtf.fp.FloatView``
``wtf::buffer::FloatBuffer``  ``wtf.buffer.FloatBuffer``
``wtf::buffer::BufferView``   ``wtf.buffer.BufferView``
============================  =========================

*************************
Python only speaks double
*************************

This is the one rule to internalize. Python's native ``float`` is a C++
``double``, and ``double`` is the only floating-point type which crosses the
Python/C++ boundary. Consequently:

- Anything constructed from Python wraps a ``double``.
- ``value()`` (and ``float()``) unwrap to a Python ``float``.
- If a type-erased object built on the C++ side is holding a ``float`` or a
  ``long double``, unwrapping it from Python raises a ``TypeError`` naming the
  held type. ``to_string()`` and ``type_name()`` still work for any held type.

The C++ instantiations over ``const`` (e.g., ``FloatView<const Float>``) are
not exposed, since Python has no notion of ``const``.

******
Floats
******

``Float`` owns its value; ``FloatView`` aliases one. A view is always built
from something that already owns a value, and writing through a view changes
the aliased value:

.. code-block:: python

   import wtf

   f = wtf.fp.Float(3.14)
   f.value()       # 3.14
   float(f)        # 3.14
   str(f)          # '3.14'
   f.type_name()   # 'double'
   f == 3.14       # True

   v = f.as_view()       # or wtf.fp.FloatView(f)
   v.set_value(1.23)
   f.value()             # 1.23

The object a view aliases is kept alive for as long as the view exists, so
``wtf.fp.Float(1.23).as_view()`` is safe.

*******
Buffers
*******

``FloatBuffer`` owns a contiguous buffer of type-erased values and is
constructed from a list of Python floats. ``BufferView`` aliases one. Both
behave like Python sequences, and indexing yields a ``FloatView`` onto the
element:

.. code-block:: python

   import wtf

   b = wtf.buffer.FloatBuffer([1.0, 2.0, 3.0])
   len(b)              # 3
   b.value()           # [1.0, 2.0, 3.0]
   b[0].value()        # 1.0
   b[1] = 9.0
   b.value()           # [1.0, 9.0, 3.0]
   b.push_back(4.0)

   v = b.as_view()     # or wtf.buffer.BufferView(b)
   v[0] = 5.0
   b.value()           # [5.0, 9.0, 3.0, 4.0]

Indexing out of range raises an ``IndexError``. ``value()`` returns a copy as
a list of Python floats; a zero-copy NumPy view is not currently provided.
