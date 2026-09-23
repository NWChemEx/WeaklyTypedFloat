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

##################
Restoring the Type
##################

WTF provides limited options for manipulating type-erased FP values. This is by
design. In particular, many applications of FP types are for linear
algebra and WTF does not want to assume a linear algebra backend. Thus,
functions taking WTF objects will need to be able to restore the type. 

**********
Exact cast
**********

Conceptually the simplest, ``wtf::cast::cast<T>``, will cast a type-erased
value to type ``T``, if ``T`` is the *exact* type held. If ``T`` doesn't match, 
``wtf::cast::cast<T>`` throws ``std::runtime_error``. 

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp
         :language: c++
         :start-after: [restoring-cast]
         :end-before: [/restoring-cast]
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../tests/python/unit_tests/doc_examples/test_doc_examples.py
         :language: python
         :start-after: [restoring-py]
         :end-before: [/restoring-py]
         :dedent: 8

Of note, ``wtf::cast::cast<T>`` can be used to get a reference to the actual
FP value, allowing the user to mutate the data in-place and/or avoid a copy.

*******
Convert
*******

``wtf::cast::cast<T>`` is somewhat limited in that the user needs to know the 
type of the held object. A weaker condition is that the user wants the wrapped
FP object back as a specific type ``T`` somewhat independently of what type it 
is actually stored as. This can be accomplished by 
``wtf::cast::convert<T, Policy>``. Here ``T`` is the type the user wants and
``Policy`` defines the rules under which the conversion should occur.

At present, WTF defines the following ``Policy``:

.. list-table::
   :header-rows: 1
   :widths: 35 65

   * - Policy
     - Behavior
   * - ``wtf::fp::policies::Match``
     - Only the exact type (same as ``cast``, but a copy).
   * - ``wtf::fp::policies::Convertible``
     - Any ``std::convertible_to``, including narrowing.
   * - ``wtf::fp::policies::Widen``
     - Only conversions that cannot lose precision.

.. literalinclude:: ../../tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp
   :language: c++
   :start-after: [restoring-convert]
   :end-before: [/restoring-convert]
   :dedent: 4

.. note::

   Since the conversions under ``wtf::cast::convert<T,Policy>`` will in general
   involve more than reinterpreting the already allocated memory,
   ``wtf::cast::convert`` will return a copy of the data.

*****
Visit
*****

The third option inverts the paradigm. Instead of trying to get the FP objects
out of the type-erased objects, ``wtf::cast::visit`` brings the computation to
the wrapped object. As the name implies, ``wtf::cast::visit`` works via the
visitor pattern, i.e., the user provides a function/functor which can be called
with arbitrary FP values. Though this may sound complicated, it is easily
accomplished with a lambda, e.g.:

.. literalinclude:: ../../tests/cxx/unit_tests/wtf/doc_examples/doc_examples.cpp
   :language: c++
   :start-after: [restoring-visit]
   :end-before: [/restoring-visit]
   :dedent: 4

.. note::

   Both ``wtf::cast::convert`` and ``wtf::cast::visit`` take a tuple of types
   to try. This argument defaults to a ``std::tuple`` built-in real FP types of
   C++. Users wanting to use additional FP types will need to ensure this
   template parameter includes the additional FP types. 

*******
Summary
*******

.. list-table::
   :header-rows: 1
   :widths: 12 25 20 43

   * - Mechanism
     - Throws on mismatch?
     - Copies or aliases?
     - Typical use
   * - ``cast``
     - Yes
     - Aliases (can mutate)
     - You know the exact type and want to read or write through it.
   * - ``visit``
     - Yes (no candidate found)
     - Neither (visitor call)
     - Generic code that doesn't know the type ahead of time.
   * - ``convert``
     - Yes (policy rejects it)
     - Always copies
     - You want a converted value, e.g. widened to ``double`` for a
       computation.

********
See also
********

For the design rationale behind these three mechanisms, see
:doc:`developer/conversion` and :doc:`developer/type_erasure`.
