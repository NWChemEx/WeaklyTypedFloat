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
#include <array>
#include <wtf/type_traits/is_convertible.hpp>
#include <wtf/type_traits/type_traits.hpp>

template<typename FromType>
void check_conversions(std::array<bool, 3> expected) {
    using namespace wtf::type_traits;
    REQUIRE(IsConvertible<FromType, float>::value == expected[0]);
    REQUIRE(IsConvertible<FromType, double>::value == expected[1]);
    REQUIRE(IsConvertible<FromType, long double>::value == expected[2]);

    using wtf::type_traits::is_convertible_v;
    REQUIRE(is_convertible_v<FromType, float> == expected[0]);
    REQUIRE(is_convertible_v<FromType, double> == expected[1]);
    REQUIRE(is_convertible_v<FromType, long double> == expected[2]);
}

TEST_CASE("IsConvertible") {
    using namespace wtf::type_traits;
    check_conversions<float>({true, true, true});
    check_conversions<double>({true, true, true});
    check_conversions<long double>({true, true, true});
    check_conversions<test_wtf::MyCustomFloat>({false, false, false});
    REQUIRE(IsConvertible<double, test_wtf::MyCustomFloat>::value);
}
