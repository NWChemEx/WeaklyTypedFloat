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

####################################
Writing APIs That Accept WTF Objects
####################################

A large motivator for creating WTF was to provide type-erased interfaces. Such
interfaces facilitate writing bindings to other languages and avoid much of the
bloat associated with using templates.

The general pattern will be to take/return type-erased FP values and "hide" the
type-specific manipulations inside the function call.

*************************
Choosing a parameter type
*************************

WTF is capable of wrapping a single FP value as well as buffers of FP values.
Additionally, WTF can own or alias the memory. This gives rise to four types:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Type
     - Use when...
   * - ``wtf::fp::Float``
     - You need to own a single erased value.
   * - ``wtf::fp::FloatView<T>``
     - You want to read or read/write a single FP value without owning it.
   * - ``wtf::buffer::FloatBuffer``
     - You need to own a whole erased buffer.
   * - ``wtf::buffer::BufferView<T>``
     - You want to read or read/write an erased buffer without owning it

Additionally, ``FloatView<T>`` and ``BufferView<T>`` come in wanting
"read only" and "read/write" flavors. Whether a view is read-only or not is
indicated by the "const"-ness of ``T``, i.e.,  ``FloatView<Float>`` will be
read/write whereas ``FloatView<const Float>`` will be read-only.

******************
API considerations
******************

Since Python uses duck-typing, this section only applies to C++.

General C++ advice for designing an API is to avoid allocating memory inside the
function. If a function will need to own the memory being provided (a common
example would be in the constructor of an object), the function should take the
object by value. This gives users the ability to move the memory into the
function, or, if the user does not want to relinquish the memory, for a copy to
automatically occur.

Most functions will only read the memory they were provided. Such functions
should take their inputs as read-only views, i.e., ``FloatView<const Float>``
and ``BufferView<const Float>``. Because of implicit conversion rules, both
``Float`` and ``FloatView<Float>`` can be passed as ``FloatView<const Float>``
objects. Similar implicit conversions exist for ``Buffer`` and ``BufferView``.

Finally, taking a ``FloatView<Float>`` or ``BufferView<Float>`` should be
reserved for when the function will mutate the data in place, i.e., when the
parameters is both an input and the result.

Return types work similarly. Member functions should prefer to return views
of their object's internal state, rather than copies, i.e., return
``FloatView``/``BufferView`` instead of ``Float``/``Buffer``. Values should
only be returned if the function is giving up ownership of the memory.

Example APIs:

.. code-block::

  class Point {
  public:
      // Point owns its coordinates' memory, so take by value
      Point(Float x, Float y, Float z);

      // Allows the user to modify the x value
      FloatView<Float> x_data();

      // Allows the user to only read the x value
      FloatView<const Float> x_data() const;

  private:
      Float m_x;
      Float m_y;
      Float m_z;
  };

  class PointView {
  public:
    // PointView aliases state, so take views
    Point(FloatView<Float> x, FloatView<Float> y, FloatView<Float> z);

  private:
    FloatView<Float> m_x;
    FloatView<Float> m_y;
    FloatView<Float> m_z;
  };
