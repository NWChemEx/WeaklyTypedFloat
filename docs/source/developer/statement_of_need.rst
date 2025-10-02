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

*******
Problem
*******

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
is that they represent the number with different numbers of bits. Consequently,
the range and precision of numbers that can be represented differs by FP type.
Unfortunately, the performance of numerics is intimately tied to the FP type
used.

In many scientific computing applications, ``double`` or
``std::complex<double>`` is used as the "universal" FP type. When higher
precision is needed the values are converted to ``long double``. When lower
precision ia acceptable, the values are converted to ``float``. This leads to
a pattern like:

.. code-block:: cpp

   std::vector<double> my_function(const std::vector<double>& data) {
       // Conversion 1: occurs if the user's data was not stored as a
       // std::vector<double>.

       // Conversion 2: double to long double
       std::vector<long double> high_precision_data(data.begin(), data.end());

        // Perform high precision computations
        // ...

        // Conversion 3: Convert back to double
        return std::vector<double>(high_precision_data.begin(),
                                   high_precision_data.end());
    }

If ``my_function`` is called from a routine that already stores its data as
``long double`` then the conversions to and from ``double`` are wasteful. To
avoid this the maintainer of ``my_function`` may maintain an overload with a
signature like
``std::vector<long double> my_function(const std::vector<long double>& data)``.
This quickly leads to a combinatorial explosion of overloads as one can
imagine that you would need one overload per FP type you want to support. The
situation is even worse if you realize that you may want to allow the user to
control the FP type used for intermediate computations and the result type
separately.

To avoid the combinatorial explosion of overloads, one may be tempted to
implement ``my_function`` as a function template. An attempt to do so may look
like:

.. code-block:: cpp

   template<typename InputT, typename IntermediateT, typename ResultT>
   std::vector<ResultT> my_function(const std::vector<InputT>& data) {
       // Conversion 1: If InputT != ResultT then convert
       std::vector<IntermediateT> intermediate_data(data.begin(), data.end());

        // Perform high precision computations
        // ...

        // Conversion 2: If IntermediateT != ResultT then convert
        return std::vector<ResultT>(intermediate_data.begin(),
                                    intermediate_data.end());
    }

While this solution removes one of the three conversions, it couples the logic
of ``my_function`` to the logic for converting the FP types.
