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

#include "../../../../test_wtf.hpp"
#include <tuple>
#include <wtf/rtti/detail_/type_model.hpp>
#include <wtf/type_traits/tuple_append.hpp>
#include <wtf/type_traits/type_traits.hpp>

using types2test = wtf::type_traits::tuple_append_t<test_wtf::all_fp_types,
                                                    std::tuple<std::nullptr_t>>;

TEMPLATE_LIST_TEST_CASE("TypeModel", "[wtf][rtti]", types2test) {
    using namespace wtf::rtti::detail_;

    std::string type_name = typeid(TestType).name();
    TypeModel<TestType> model(type_name);
    TypeModel<const TestType> const_model(type_name);

    SECTION("name_") {
        REQUIRE(model.name() == type_name);
        REQUIRE(const_model.name() == "const " + type_name);
    }

    SECTION("is_const_") {
        REQUIRE_FALSE(model.is_const());
        REQUIRE(const_model.is_const());
    }

    SECTION("is_nullptr_") {
        if constexpr(std::is_same_v<TestType, std::nullptr_t>) {
            REQUIRE(model.is_nullptr());
            REQUIRE(const_model.is_nullptr());
        } else {
            REQUIRE_FALSE(model.is_nullptr());
            REQUIRE_FALSE(const_model.is_nullptr());
        }
    }

    SECTION("make_const_") {
        REQUIRE(model.make_const()->are_equal(const_model));
        REQUIRE(const_model.make_const()->are_equal(const_model));
    }

    SECTION("are_equal_") {
        REQUIRE(model.are_equal(model));
        REQUIRE_FALSE(model.are_equal(const_model));

        REQUIRE_FALSE(const_model.are_equal(model));
        REQUIRE(const_model.are_equal(const_model));
    }
}
