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
Why do we need WTF?
###################

**********
Motivation
**********

The standard C++ library already includes a host of FP types including:

-  ``float``
-  ``double``
-  ``long double``
-  ``std::complex<float>``
-  ``std::complex<double>``
-  ``std::complex<long double>``

...and that's before we discuss the types introduced by C++23. Conceptually,
these types are all used to do the same thing: represent real (or more
generally complex) numbers. In practice, the difference among these FP types
is that they represent the number with different amounts of bits. Consequently,
the range and precision of numbers that can be represented differs by FP type.
Unfortunately, the performance of numerics is intimately tied to the FP type
used.

.. note::

   For brevity we will focus on the real FP types. Everything we say here
   readily generalizes to complex FP types as well.

A fundamental problem in the software engineering of scientific libraries is
dealing with FP types. Historically, for simplicity many scientific libraries 
have assumed ``double`` as the FP type at all interfaces. If the user is
storing their data as ``float``, they must convert it to ``double`` to call the
interface, and then convert the result back to ``float`` after the call.
Admittedly, this is why many scientific libraries provide overloads for other
FP types, but this in turn requires the developer to maintain one interface
per FP type. 

C++ libraries can avoid the need to support multiple overloads by using 
templates. As long as the user of the library is calling the function from C++, 
this solution works well. However, it is becoming increasingly important to
be able to interface scientific software to other languages (e.g., Python). In
most cases, interfacing is done through a C-like interface, precluding the use
of templates.

*****************
Problem Statement
*****************

Scientific software developers increasingly need to support multiple FP types.
At the same time, they want their software to be callable from multiple
languages. Unfortunately, this precludes the use of templates at user-facing
interfaces. 

*****
Goals
*****

- Provide a series of abstractions that are capable of holding
  arbitrary FP types.
- Make it easy for users of WTF to compose algorithms with these abstractions.  
- Ensure that the abstractions can be used in a performant manner.