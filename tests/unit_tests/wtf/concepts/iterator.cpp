/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../../test_wtf.hpp"
#include <wtf/concepts/iterator.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("FPIterator", "[wtf][concepts]",
                        test_wtf::all_fp_types) {
    using vector_type        = std::vector<TestType>;
    using vector_iterator    = typename vector_type::iterator;
    using pointer_type       = TestType*;
    using const_pointer_type = const TestType*;

    STATIC_REQUIRE(FPIterator<vector_iterator>);
    STATIC_REQUIRE(FPIterator<pointer_type>);
    STATIC_REQUIRE(FPIterator<const_pointer_type>);

    STATIC_REQUIRE_FALSE(FPIterator<int>);
}
