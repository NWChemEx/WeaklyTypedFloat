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

#####################################
FloatBuffer and BufferView Containers
#####################################

``wtf::FloatBuffer`` and ``wtf::BufferView`` both type-erase a buffer of
floating-point values. In practice, array-of-structure objects like
``std::vector<Float>`` are performance inefficient and the point of
``wtf::FloatBuffer`` and ``wtf::BufferView`` is to provide an abstraction over
the underlying memory so that user code looks/behave like it uses
``std::vector<Float>`` as much as possible. The two classes exist to serve two
different relationships to
the underlying memory (see :doc:`scope` for the original design goals):

- ``wtf::FloatBuffer`` **owns** the memory it type-erases.
- ``wtf::BufferView`` **aliases** memory owned by someone else (a
  ``FloatBuffer``, a ``std::vector``, a raw pointer, ...).

Both are implemented with the same "Interface"/"Holder"/"Model" type-erasure
pattern described in :doc:`type_erasure`.

*********************
Design Considerations
*********************

FloatBuffer is vector-like, not array-like
==========================================

Early on, ``FloatBuffer`` was a fixed-size once constructed -- more like
``std::array<T, N>`` than ``std::vector<T>``. This led to a pattern where the
user had to: 1. Create a ``std::vector<Float>`` and then 2. Move the
``std::vector<Float>`` into the ``FloatBuffer``. This felt like a workaround
to get around the fact that ``FloatBuffer`` was not vector-like. So we opted
to make ``FloatBuffer`` vector-like, so that it can be constructed empty and
then grow as needed.

BufferView is span-like, not vector-like
========================================

``BufferView`` intentionally does **not** get ``push_back``/``reserve``.
A view aliases memory it does not own; there is nothing for it to grow into
without invalidating whatever it is aliasing (and, if it aliased a
``std::vector`` that reallocated, the view would be left dangling regardless).
So ``BufferView`` stays span-like: a fixed-size, non-owning window onto
another object's contiguous memory.

***********
Misc. Notes
***********

Implementing push_back for a type-erased T
==========================================

We want ``push_back`` to be able to accept raw floating-point values (e.g.
``double``), ``wtf::Float``, or ``wtf::FloatView``. In each case the
implementation should work out what concrete C++ type to store. For the raw-
value overload this is trivial -- the type is a template parameter, known at
compile time. For ``Float``/ ``FloatView`` it is only known at runtime.

The easy type-erasure scenario is when ``*this`` already holds a buffer: the
held buffer's element type is known (via RTTI), so the incoming ``Float``/
``FloatView`` just needs to be un-erased to that type (via ``fp::float_cast``,
throwing ``std::runtime_error`` on a mismatch, the same throw-on-mismatch idiom
used everywhere else in WTF) and appended.

The hard case is appending a ``Float``/ ``FloatView`` to an *empty*
``FloatBuffer`` -- because we need to work out what type the buffer should hold.
The first solution we tried relies on the fact that the ``Float``/
``FloatView``'s own internal model knows the held type, so if it can build the
 ``ContiguousModel`` directly we would not need a candidate list. The problem is
 that we have a firm ``buffer`` depends on ``fp`` architecture, i.e., the
 ``fp`` module needs to be fully resolved before the ``buffer`` module can be
 compiled. Having ``Float``/ ``FloatView``'s model build the ``ContiguousModel``
 directly would require us to resolve part of the ``buffer`` module before the
 ``fp`` module can be compiled, which is a circular dependency.

The current solution is to have the caller supply a ``TupleType`` of candidate
types,  exactly the same mechanism :doc:`operations` describes for dispatching
operations.
