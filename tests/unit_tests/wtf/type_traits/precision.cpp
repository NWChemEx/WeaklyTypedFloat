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
#include <wtf/type_traits/precision.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::type_traits;

TEST_CASE("Precision") {
    SECTION("float") {
        STATIC_REQUIRE(Precision<float>::value == 6);
        STATIC_REQUIRE(precision_v<float> == 6);
    }
    SECTION("double") {
        STATIC_REQUIRE(Precision<double>::value == 15);
        STATIC_REQUIRE(precision_v<double> == 15);
    }
    SECTION("long double") {
        STATIC_REQUIRE(Precision<long double>::value >= 15);
        STATIC_REQUIRE(precision_v<long double> >= 15);
    }
    SECTION("MyCustomFloat") {
        STATIC_REQUIRE(Precision<test_wtf::MyCustomFloat>::value == 0);
        STATIC_REQUIRE(precision_v<test_wtf::MyCustomFloat> == 0);
    }
}
