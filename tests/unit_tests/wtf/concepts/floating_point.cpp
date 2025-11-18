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
#include <wtf/concepts/floating_point.hpp>
#include <wtf/fp/float.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("UnmodifiedFloatingPoint", "[wtf][concepts]",
                        test_wtf::all_fp_types) {
    STATIC_REQUIRE(UnmodifiedFloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<wtf::fp::Float>);
}

TEMPLATE_LIST_TEST_CASE("FloatingPoint", "[wtf][concepts]",
                        test_wtf::all_fp_types) {
    STATIC_REQUIRE(FloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<TestType&>);
    STATIC_REQUIRE(FloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const TestType&>);
    STATIC_REQUIRE(FloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile TestType&>);
    STATIC_REQUIRE(FloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile TestType&>);
}

TEMPLATE_LIST_TEST_CASE("ConstFloatingPoint", "[wtf][concepts]",
                        test_wtf::all_fp_types) {
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType&>);
    STATIC_REQUIRE(ConstFloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const TestType&>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType&>);
    STATIC_REQUIRE(ConstFloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile TestType&>);
}
