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
Type-Erasure in WTF
###################

The goal of WTF is to have the user work with objects like ``wtf::Float``,
``wtf::FloatBuffer`` and not say ``wtf::Float<double>``, but at the same time
ensure that operations are done in a type-safe manner. This is achieved
through a pattern known as "type-erasure". This page documents how we pull
type-erasure off in WTF.

***********
Terminology
***********

Type-erasure involves three classes.

1. The "Interface" class. This is the class that the user interacts with.
   In WTF these are ``wtf::Float`` and ``wtf::Buffer``.
2. The "Holder" class. This is an abstract base class that defines the
   interactions between the "Interface" class and the actual data. In WTF this
   is ``wtf::detail::FloatHolder`` and ``wtf::detail::BufferHolder``.
3. The "Model" class. This is a class templated on the type of data being
   held. It derives from the "Holder" class and implements a model for data of
   type ``T``. In WTF these are ``wtf::detail::FloatModel<T>`` and
   ``wtf::detail::BufferModel<T>``.