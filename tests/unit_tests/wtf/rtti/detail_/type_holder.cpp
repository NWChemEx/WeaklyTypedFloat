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
#include <wtf/rtti/detail_/type_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::rtti::detail_;
using TypeModelf = TypeModel<float>;
using TypeModeld = TypeModel<double>;
using TypeModell = TypeModel<long double>;

TEMPLATE_LIST_TEST_CASE("TypeHolder", "[wtf][rtti]", test_wtf::all_fp_types) {
    std::string type_name = typeid(TestType).name();
    TypeModel<TestType> model(type_name);
    TypeModel<const TestType> const_model(type_name);

    TypeHolder& model_holder       = model;
    TypeHolder& const_model_holder = const_model;

    SECTION("get_model") {
        auto m1 = model_holder.get_model();
        auto m2 = const_model_holder.get_model();

        REQUIRE(m1->are_equal(model));
        REQUIRE(m2->are_equal(const_model));

        // Must return the SAME pointer each time
        REQUIRE(m1 == model_holder.get_model());
        REQUIRE(m2 == const_model_holder.get_model());
    }

    SECTION("name") {
        REQUIRE(model_holder.name() == type_name);
        REQUIRE(const_model_holder.name() == "const " + type_name);
    }

    SECTION("precision") {
        constexpr auto corr = wtf::type_traits::precision_v<TestType>;
        REQUIRE(model_holder.precision() == corr);
        REQUIRE(const_model_holder.precision() == corr);
    }

    SECTION("is_const") {
        REQUIRE_FALSE(model_holder.is_const());
        REQUIRE(const_model_holder.is_const());
    }

    SECTION("make_const") {
        auto cm = model_holder.make_const();
        REQUIRE(cm->are_equal(const_model));
        REQUIRE(cm->is_const());
        REQUIRE(const_model.make_const()->are_equal(const_model));
    }

    SECTION("are_equal") {
        REQUIRE(model_holder.are_equal(model));
        REQUIRE(const_model_holder.are_equal(const_model));

        REQUIRE_FALSE(model_holder.are_equal(const_model));
        REQUIRE_FALSE(const_model_holder.are_equal(model));
    }

    SECTION("is_implicitly_convertible_to") {
        using types = test_wtf::all_fp_types;
        REQUIRE(model_holder.is_implicitly_convertible_to<types>(model));
        REQUIRE(model_holder.is_implicitly_convertible_to<types>(const_model));

        REQUIRE(
          const_model_holder.is_implicitly_convertible_to<types>(const_model));
        REQUIRE_FALSE(
          const_model_holder.is_implicitly_convertible_to<types>(model));
    }
}
