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
#include <wtf/type_traits/type_name.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::type_traits;
using namespace test_wtf;

TEST_CASE("TypeName") {
    REQUIRE(TypeName<float>::value == std::string("float"));
    REQUIRE(type_name_v<float> == std::string("float"));
    REQUIRE(TypeName<double>::value == std::string("double"));
    REQUIRE(type_name_v<double> == std::string("double"));
    REQUIRE(TypeName<long double>::value == std::string("long double"));
    REQUIRE(type_name_v<long double> == std::string("long double"));

    std::string custom_corr = "test_wtf::MyCustomFloat";
    REQUIRE(TypeName<MyCustomFloat>::value == custom_corr);
    REQUIRE(type_name_v<MyCustomFloat> == custom_corr);
}
