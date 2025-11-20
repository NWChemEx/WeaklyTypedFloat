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

#########################
What is the Scope of WTF?
#########################

The goal of WTF is to provide a set of abstractions that enable users to
write APIs that work with arbitrary floating point types without having to
rely on templates. At the same time, we also realize that floating-point types
are often used in numerical algorithms where they are often subjected to
linear algebra operations. WTF does NOT want to implement linear algebra! It is
thus crucial that we decide where the line between linear algebra and WTF is.

****************************
Needed Concepts/Abstractions
****************************

WTF will need to minimally provide the user with a class representing a type-
erased FP value. In practice users will rarely want to work with a single value
so we will also need to provide a way to work with collections of these values,
of particular note is that users will want to work with collections of these
values that are stored in contiguous memory. To provide an object-oriented API,
it must be possible for the user to grab an element from the collection in a
way that the element is assignable, but still stored in the full collection
(e.g., think of it as a reference or view of the element). Similarly, it must
be possible to grab a sub-collection of the full collection in a way that the
sub-collection is assignable, but still stored in the full collection. The
latter also enables the user to interface already allocated memory with WTF.

In summary:

- ``wtf::Float`` class that holds a type-erased floating point value. Owns the
  underlying value.
- ``wtf::FloatBuffer`` class that acts like it holds a series of ``wtf::Float``
  objects, but actually unwraps the underlying type into contiguous memory. Owns
  the underlying memory.
- ``wtf::FloatView`` class that acts like it is aliasing a ``wtf::Float``
  object, but actually wraps a pointer to the raw memory.
- ``wtf::FloatBufferView`` class that acts like an alias to a
  ``wtf::FloatBuffer`` object, but actually wraps pointer(s) to the contiguous
  memory.

In practice, we will also need ``const`` versions of the view classes (views
end up working like iterators).

**********************
Fundamental Operations
**********************

Under typical circumstances all C++ objects should provide:

- Default constructors.
- Copy and move constructors.
  - Deep copy for objects that own memory. Shallow for those that alias.
- Copy and move assignment operators.
  - Deep copy for objects that own memory. Shallow for those that alias.
- Equality/Inequality operators.

  - Exact equality. Good for testing that state is set correctly. Not used for
    checking results of numerical calculations (vide infra).

Since these will be numerical types it will also be nice if our objects
provided:

- Operations to establish order (e.g., ``<``, ``>``, etc...).
- The ability to determine approximate equality.
  - User should be able to control relative and absolute tolerances.
- A way to print the value to a stream (e.g., ``operator<<``).

***********
Conversions
***********

To support the above we must be able to convert between types. Depending on the
circumstance we may not always want the same conversion. For example, if we
are afraid of loosing precision we will want all conversions to use the most
precise type. On the other hand, if we are optimizing for speed we may want
all conversions to use the least precise type.

****************
Maybe We Support
****************

- Element-wise arithmetic operations (e.g., ``+``, ``-``, ``*``, ``/``).

  - Implementing these in a performant manner may require us to use specialized
    math libraries, which in turn makes this start to look like linear
    algebra.
