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
#include <wtf/concepts/modifiers.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("IsPointer", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(IsPointer<TestType>);
    STATIC_REQUIRE(IsPointer<TestType*>);
    STATIC_REQUIRE_FALSE(IsPointer<TestType&>);
    STATIC_REQUIRE_FALSE(IsPointer<const TestType>);
    STATIC_REQUIRE(IsPointer<const TestType*>);
    STATIC_REQUIRE_FALSE(IsPointer<const TestType&>);
    STATIC_REQUIRE_FALSE(IsPointer<volatile TestType>);
    STATIC_REQUIRE(IsPointer<volatile TestType*>);
    STATIC_REQUIRE_FALSE(IsPointer<volatile TestType&>);
    STATIC_REQUIRE_FALSE(IsPointer<const volatile TestType>);
    STATIC_REQUIRE(IsPointer<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(IsPointer<const volatile TestType&>);
}

TEMPLATE_LIST_TEST_CASE("IsReference", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(IsReference<TestType>);
    STATIC_REQUIRE_FALSE(IsReference<TestType*>);
    STATIC_REQUIRE(IsReference<TestType&>);
    STATIC_REQUIRE_FALSE(IsReference<const TestType>);
    STATIC_REQUIRE_FALSE(IsReference<const TestType*>);
    STATIC_REQUIRE(IsReference<const TestType&>);
    STATIC_REQUIRE_FALSE(IsReference<volatile TestType>);
    STATIC_REQUIRE_FALSE(IsReference<volatile TestType*>);
    STATIC_REQUIRE(IsReference<volatile TestType&>);
    STATIC_REQUIRE_FALSE(IsReference<const volatile TestType>);
    STATIC_REQUIRE_FALSE(IsReference<const volatile TestType*>);
    STATIC_REQUIRE(IsReference<const volatile TestType&>);
}

TEMPLATE_LIST_TEST_CASE("ConstQualified", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(ConstQualified<TestType>);
    STATIC_REQUIRE_FALSE(ConstQualified<TestType*>);
    STATIC_REQUIRE_FALSE(ConstQualified<TestType&>);
    STATIC_REQUIRE(ConstQualified<const TestType>);
    STATIC_REQUIRE(ConstQualified<const TestType*>);
    STATIC_REQUIRE(ConstQualified<const TestType&>);
    STATIC_REQUIRE_FALSE(ConstQualified<volatile TestType>);
    STATIC_REQUIRE_FALSE(ConstQualified<volatile TestType*>);
    STATIC_REQUIRE_FALSE(ConstQualified<volatile TestType&>);
    STATIC_REQUIRE(ConstQualified<const volatile TestType>);
    STATIC_REQUIRE(ConstQualified<const volatile TestType*>);
    STATIC_REQUIRE(ConstQualified<const volatile TestType&>);
}

TEMPLATE_LIST_TEST_CASE("VolatileQualified", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(VolatileQualified<TestType>);
    STATIC_REQUIRE_FALSE(VolatileQualified<TestType*>);
    STATIC_REQUIRE_FALSE(VolatileQualified<TestType&>);
    STATIC_REQUIRE_FALSE(VolatileQualified<const TestType>);
    STATIC_REQUIRE_FALSE(VolatileQualified<const TestType*>);
    STATIC_REQUIRE_FALSE(VolatileQualified<const TestType&>);
    STATIC_REQUIRE(VolatileQualified<volatile TestType>);
    STATIC_REQUIRE(VolatileQualified<volatile TestType*>);
    STATIC_REQUIRE(VolatileQualified<volatile TestType&>);
    STATIC_REQUIRE(VolatileQualified<const volatile TestType>);
    STATIC_REQUIRE(VolatileQualified<const volatile TestType*>);
    STATIC_REQUIRE(VolatileQualified<const volatile TestType&>);
}

TEMPLATE_LIST_TEST_CASE("Unmodified", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(Unmodified<TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<TestType&>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType&>);
    STATIC_REQUIRE_FALSE(Unmodified<volatile TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<volatile TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<volatile TestType&>);
    STATIC_REQUIRE_FALSE(Unmodified<const volatile TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<const volatile TestType&>);
}
