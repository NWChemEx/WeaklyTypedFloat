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
#include <wtf/fp/detail_/float_view_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace test_wtf;
using namespace wtf::fp;

TEMPLATE_LIST_TEST_CASE("FloatViewHolder", "[wtf]", all_fp_types) {
    using float_t = TestType;

    // Different type
    constexpr bool is_float = std::is_same_v<float_t, float>;
    using other_type        = std::conditional_t<is_float, double, float>;

    using holder_t       = wtf::fp::detail_::FloatViewHolder<Float>;
    using const_holder_t = wtf::fp::detail_::FloatViewHolder<const Float>;
    using model_t        = wtf::fp::detail_::FloatViewModel<float_t>;
    using const_model_t  = wtf::fp::detail_::FloatViewModel<const float_t>;

    float_t val = 3.14;
    model_t m(&val);
    holder_t& h = m;

    const_model_t cm(&val);
    const_holder_t& ch = cm;

    SECTION("clone") {
        REQUIRE(h.clone()->are_equal(m));
        REQUIRE(ch.clone()->are_equal(cm));
    }

    SECTION("const_clone") {
        REQUIRE(h.const_clone()->are_equal(cm));
        REQUIRE(ch.const_clone()->are_equal(cm));
    }

    SECTION("change_value") {
        float_t val2 = 0;
        model_t m2(&val2);
        h.change_value(m2);
        REQUIRE(m.data() == &val);

        const_model_t cm2(&val2);
        REQUIRE_THROWS_AS(ch.change_value(cm2), std::runtime_error);

        other_type val3 = 3.14;
        detail_::FloatViewModel<other_type> m4(&val3);
        REQUIRE_THROWS_AS(h.change_value(m4), std::invalid_argument);
    }

    SECTION("type()") {
        REQUIRE(h.type() == wtf::rtti::wtf_typeid<float_t>());
        REQUIRE(ch.type() == wtf::rtti::wtf_typeid<const float_t>());
    }

    SECTION("is_type") {
        REQUIRE(h.template is_type<float_t>());
        REQUIRE_FALSE(h.template is_type<const float_t>());
        REQUIRE_FALSE(h.template is_type<other_type>());
        REQUIRE_FALSE(h.template is_type<const other_type>());

        REQUIRE_FALSE(ch.template is_type<float_t>());
        REQUIRE(ch.template is_type<const float_t>());
        REQUIRE_FALSE(ch.template is_type<other_type>());
        REQUIRE_FALSE(ch.template is_type<const other_type>());
    }

    SECTION("are_equal") {
        model_t m2(&val);
        holder_t& h2 = m2;
        REQUIRE(h.are_equal(h2));

        const_model_t cm2(&val);
        const_holder_t& ch2 = cm2;
        REQUIRE(ch.are_equal(ch2));

        // Different values
        float_t val2 = 1.23;
        model_t m3(&val2);
        holder_t& h3 = m3;
        REQUIRE_FALSE(h.are_equal(h3));

        const_model_t cm3(&val2);
        const_holder_t& ch3 = cm3;
        REQUIRE_FALSE(ch.are_equal(ch3));

        // Different types
        other_type val3 = 3.14;
        detail_::FloatViewModel<other_type> m4(&val3);
        holder_t& h4 = m4;
        REQUIRE_FALSE(h.are_equal(h4));

        detail_::FloatViewModel<const other_type> cm4(&val3);
        const_holder_t& ch4 = cm4;
        REQUIRE_FALSE(ch.are_equal(ch4));

        // Different const-ness
        REQUIRE_FALSE(h.are_equal(ch));
    }

    SECTION("to_string") {
        if constexpr(wtf::concepts::StreamInsertable<float_t>) {
            std::stringstream ss;
            ss << val;
            REQUIRE(h.to_string() == ss.str());
            REQUIRE(ch.to_string() == ss.str());
        } else {
            REQUIRE(h.to_string() == "<unprintable float>");
            REQUIRE(ch.to_string() == "<unprintable float>");
        }
    }
}
