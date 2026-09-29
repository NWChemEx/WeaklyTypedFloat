.. Copyright 2026 NWChemEx-Project
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

##########
Quickstart
##########

This page demonstrates some of the basic functionality of WTF. Subsequent pages
explore more specialized (and performant) mechanisms for similar operations.

.. note::

   C++ code is found in
   ``tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp`` and the Python
   code is found in
   ``tests/python/unit_tests/doc_examples/test_doc_examples.py``.

******************
A Float, un-erased
******************

``wtf::fp::Float`` (``wtf.fp.Float`` in Python) wraps a single floating-point
value and hides its concrete type:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp
         :language: c++
         :start-after: [quickstart-float]
         :end-before: [/quickstart-float]
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../tests/python/unit_tests/doc_examples/test_doc_examples.py
         :language: python
         :start-after: [quickstart-float-py]
         :end-before: [/quickstart-float-py]
         :dedent: 8

It is worth noting that the float is always stored as the FP type provided to
WTF, i.e., ``to_string`` is a convient way for us to show the value and not
indicative of how WTF stores the float.

************************
A FloatBuffer, un-erased
************************

``wtf::buffer::FloatBuffer`` (``wtf.buffer.FloatBuffer`` in Python) does the
same thing for a contiguous buffer of values:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp
         :language: c++
         :start-after: [quickstart-buffer]
         :end-before: [/quickstart-buffer]
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../tests/python/unit_tests/doc_examples/test_doc_examples.py
         :language: python
         :start-after: [quickstart-buffer-py]
         :end-before: [/quickstart-buffer-py]
         :dedent: 8

``wtf::cast::cast<T>`` (``.value()`` in Python) is only one of several ways
to get a concrete type back out -- see :doc:`restoring_the_type` for the
others.

**********
Next steps
**********

- :doc:`writing_apis` -- how to design a function signature around WTF's
  type-erased objects.
- :doc:`restoring_the_type` -- the different ways to get a concrete type
  back out of one, and when to use each.
