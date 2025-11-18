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
Architecture of WTF
###################

****
FAQs
****

- There's a lot of repetition between ``FloatHolder`` and ``FloatViewHolder``,
  why can't we combine them?

  - In hindsight, we could define a class ``FloatHolderAPI<T>`` that is
    templated on the type of ``Float`` that will hold it. ``FloatModel`` would
    always derive from ``FloatHolderAPI<Float>`` whereas ``FloatViewModel``
    would derive from ``FloatHolderAPI<Float>`` or
    ``FloatHolderAPI<const Float>`` depending on the const-ness of the type
    argument to ``FloatViewModel``. The holder classes were already written
    when this realization occurred, so it was not worth the effort to refactor
    them.
