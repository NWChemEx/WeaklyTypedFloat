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
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/type_traits.hpp>

// The need for "typename" makes these pretty long. To clean up the test we
// define a macro for aliasing the members of the traits class.
#define UNPACK_TRAITS(traits)                                   \
    using value_type       = typename traits::value_type;       \
    using unqualified_type = typename traits::unqualified_type; \
    using const_value_type = typename traits::const_value_type; \
    using reference        = typename traits::reference;        \
    using const_reference  = typename traits::const_reference;  \
    using pointer          = typename traits::pointer;          \
    using const_pointer    = typename traits::const_pointer

TEMPLATE_LIST_TEST_CASE("float_traits", "[type_traits]",
                        test_wtf::all_fp_types) {
    SECTION("unqualified type") {
        using traits_type = wtf::type_traits::float_traits<TestType>;
        UNPACK_TRAITS(traits_type);
        STATIC_REQUIRE(std::is_same_v<value_type, TestType>);
        STATIC_REQUIRE(std::is_same_v<unqualified_type, TestType>);
        STATIC_REQUIRE(std::is_same_v<const_value_type, const TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
        STATIC_REQUIRE_FALSE(traits_type::is_const);
    }

    SECTION("const-qualified type") {
        using traits_type = wtf::type_traits::float_traits<const TestType>;
        UNPACK_TRAITS(traits_type);
        STATIC_REQUIRE(std::is_same_v<value_type, const TestType>);
        STATIC_REQUIRE(std::is_same_v<unqualified_type, TestType>);
        STATIC_REQUIRE(std::is_same_v<const_value_type, const TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, const TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
        STATIC_REQUIRE(traits_type::is_const);
    }
}

#undef UNPACK_TRAITS
