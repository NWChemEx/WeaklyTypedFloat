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

#################
Writing a Visitor
#################

To avoid assuming a particular set of operations WTF relies on the visitor
pattern. This page should eventually be a tutorial, but for now is just a set
of notes.

Assume that ``visitor`` is an object of type ``T``. The dispatch function will
additionally require a ``std::tuple<FPList...>`` where ``FPList...`` is a list
of the floating-point types you want to be able to support. Then assume we are
trying to dispatch based on the wrapped types of ``N`` ``FloatBuffer`` objects.

- ``T`` must be a callable type, i.e., define ``operator()``. Functions,
  lambdas, and functors all satisfy this criterion
- ``operator()`` may be overloaded. If it is overloaded, the usual C++ overload
   resolution rules will be used to select the appropriate overload.
- Dispatching considers the type of the floating point buffers and the number of
   floating point buffers.
- ``operator()`` overloads must accent ``N`` parameters.
- ``operator()`` must possess an overload capable of supporting any permutation
  with replacement of the types in ``FPLists...``.

  - The easiest way to satisfy this is to have a templated overload with ``N``
    template parameters (one for the type of each positional argument).

- Each ``FloatBuffer`` will be unwrapped to a ``std::span<U>`` object where
  ``U`` is the type of the wrapped floating point objects.
