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

###################
Why Do We Need WTF?
###################

Executive summary:

#. Lots of floating-point (FP) types.
#. Some algorithms need to dispatch based on FP type.
#. Many (most?) algorithms don't need to know the type.
#. Want a way to "hide" the type from the interface of algorithms that don't
   need to know the type.

**********
Motivation
**********

The standard C++ library already includes a host of floating-point (FP) types
including:

-  ``float``
-  ``double``
-  ``long double``
-  ``std::complex<float>``
-  ``std::complex<double>``
-  ``std::complex<long double>``

...and that's before we discuss the types introduced by C++23. The number of
FP types ultimately becomes unbounded when we additionally consider user-
defined FP types, e.g., automatic differentiation, error tracking. At a high-
level these types are all representations of the same thing real/complex numbers
(or more generally elements of a scalar field). However, the seemingy plethora
of types is not without reason as algorithms may look quite different for each
FP type:

#. Different needs for preserving precision. Example, accumulating
   intermediates in a higher precision type.
#. Iterative algorithms may require different stopping points for different
   types.
#. Underflows/overflows require different guarding.
#. Operation cost varies by type e.g., native vs. emulated instructions.
#. Type's size may change whether algorithm is memory or compute bound.
#. Can be extra steps, e.g., complex-conjugation.

In practice, the distinctions among most of these types is either low-level
(e.g., the number of bytes they leverage) or independent of the primary
abstraction (i.e., that they are real/complex numbers). This means that higher-
level algorithms usually are written the same way, regardless of the floating-
point type. Put another way, for many algorithms the floating-point type is
an implementation detail that should not be part of the public interface.

In C++, we usually use templated code in order to support arbitrary floating
point types. For example:

.. code::

   template<typename T>
   auto add(T lhs, T rhs){
       return lhs + rhs;
   }

allows us to add two FP objects of type ``T``, so long as ``operator+(T, T)`` is
defined. This example can be extended to mixes of FP types by allowing ``lhs``
and ``rhs`` to have different types and then requiring either
``operator+(T, U)`` to be defined, or that implicit conversions are defined.

Taken to its logical conclusion, the above suggests that in order for a C++
library to support arbitrary FP types, all functions involving FP types must be
templated. While templated libraries are popular options to bypass build system
issues (e.g., binary compatability) they have a number of drawbacks:

#. Longer compilation times.
#. Larger binary sizes.
#. Type must be known at compile time.
#. No application binary interface (ABI).
#. Verbose compiler errors.
#. Intimidating for C++ novices.

The lack of ABIs is a blocker if we want to leverage the library from other
languages (notably Python which relies on the C ABI). The real problem is that
templates do not actually solve the problem of avoiding FP types in the
interface, they instead automate the creation of interfaces with different FP
types.

*****************
Problem Statement
*****************

We want a mechanism for decoupling high-level algorithms from the floating-point
type used to represent real/complex numbers. The mechanism must allow libraries
to provide stable ABIs, while also allowing performance critical algorithms to
dispatch on the FP type.

*****
Goals
*****

- Provide a series of abstractions that are capable of holding arbitrary FP
  types.
- Make it easy for users of WTF to compose algorithms with these abstractions.
- Ensure that the user can extend WTF to support their own custom FP types
  without needing to modify the WTF source.
- Ensure that the abstractions can be used in a performant manner.
