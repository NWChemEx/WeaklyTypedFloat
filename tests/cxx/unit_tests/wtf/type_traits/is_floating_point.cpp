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
#include <wtf/type_traits/is_floating_point.hpp>

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (true)", "[type_traits]",
                        test_wtf::all_fp_types) {
    STATIC_REQUIRE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE(wtf::type_traits::is_floating_point_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (false)", "[type_traits]",
                        test_wtf::not_fp_types) {
    STATIC_REQUIRE_FALSE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE_FALSE(wtf::type_traits::is_floating_point_v<TestType>);
}
