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

using namespace wtf::fp::detail_;

TEMPLATE_LIST_TEST_CASE("FloatViewModel", "[float_model]",
                        test_wtf::all_fp_types) {
    using float_t       = TestType;
    using model_t       = FloatViewModel<float_t>;
    using const_model_t = FloatViewModel<const float_t>;

    float_t val = 3.14;
    model_t m(&val);
    const_model_t cm(&val);

    SECTION("Ctors and assignment") {
        SECTION("Value ctor") {
            REQUIRE(m.get_value() == val);
            REQUIRE(cm.get_value() == val);
            REQUIRE(m.data() == &val);
            REQUIRE(cm.data() == &val);
        }
        SECTION("Copy ctor") {
            model_t m2(m);
            const_model_t cm2(cm);
            REQUIRE(m2 == m);
            REQUIRE(cm2 == cm);
            REQUIRE(m2.data() == &val);
            REQUIRE(cm2.data() == &val);
        }
        SECTION("Copy assignment") {
            float_t zero(0);
            model_t m2(&zero);
            const_model_t cm2(&zero);
            auto pm2  = &(m2 = m);
            auto pcm2 = &(cm2 = cm);
            REQUIRE(m2.get_value() == val);
            REQUIRE(cm2.get_value() == val);
            REQUIRE(pm2 == &m2);   // Returns *this
            REQUIRE(pcm2 == &cm2); // Returns *this
        }
        SECTION("Move ctor") {
            model_t m2(m);
            const_model_t cm2(cm);
            model_t m3(std::move(m2));
            const_model_t cm3(std::move(cm2));
            REQUIRE(m3 == m);
            REQUIRE(cm3 == cm);
        }
        SECTION("Move assignment") {
            float_t zero(0);
            model_t m2(&zero);
            const_model_t cm2(&zero);
            model_t m3(m);
            const_model_t cm3(cm);
            auto pm2  = &(m2 = std::move(m3));
            auto pcm2 = &(cm2 = std::move(cm3));
            REQUIRE(m2 == m);
            REQUIRE(cm2 == cm);
            REQUIRE(pm2 == &m2);   // Returns *this
            REQUIRE(pcm2 == &cm2); // Returns *this
        }
    }

    SECTION("get_value") {
        REQUIRE(m.get_value() == val);
        REQUIRE(&m.get_value() == &val);
        REQUIRE(cm.get_value() == val);
        REQUIRE(&cm.get_value() == &val);
    }

    SECTION("set_value") {
        float_t new_val = 1.23;
        m.set_value(new_val);
        REQUIRE(m.get_value() == new_val);
        REQUIRE_THROWS_AS(cm.set_value(new_val), std::runtime_error);
    }

    SECTION("data()") {
        auto p = m.data();
        REQUIRE(p == &val);
        auto p2 = cm.data();
        REQUIRE(p2 == &val);
    }

    SECTION("data() const") {
        auto p  = std::as_const(m).data();
        auto p2 = std::as_const(cm).data();
        REQUIRE(p == &val);
        REQUIRE(*p == val);
        REQUIRE(p2 == &val);
        REQUIRE(*p2 == val);
    }

    SECTION("operator==") {
        model_t m2(&val);
        REQUIRE(m == m2);

        const_model_t cm2(&val);
        REQUIRE(cm == cm2);

        // Different value
        float_t other_val(1.23);
        model_t m3(&other_val);
        REQUIRE_FALSE(m == m3);

        const_model_t cm3(&other_val);
        REQUIRE_FALSE(cm == cm3);
    }

    SECTION("operator!=") {
        model_t m2(&val);
        REQUIRE_FALSE(m != m2);

        const_model_t cm2(&val);
        REQUIRE_FALSE(cm != cm2);

        float_t other_val(1.23);
        model_t m3(&other_val);
        REQUIRE(m != m3);

        const_model_t cm3(&other_val);
        REQUIRE(cm != cm3);
    }

    SECTION("swap") {
        float_t other_val(1.23);
        model_t m2(&other_val);
        m.swap(m2);
        REQUIRE(m.get_value() == other_val);
        REQUIRE(m2.get_value() == val);

        const_model_t cm2(&other_val);
        cm.swap(cm2);
    }

    SECTION("are_equal_") {
        model_t m2(&val);
        REQUIRE(m.are_equal(m2));

        const_model_t cm2(&val);
        REQUIRE(cm.are_equal(cm2));

        // Different value
        float_t other_val(1.23);
        model_t m3(&other_val);
        REQUIRE_FALSE(m.are_equal(m3));

        const_model_t cm3(&other_val);
        REQUIRE_FALSE(cm.are_equal(cm3));

        // Different type
        using other_type =
          std::conditional_t<std::is_same_v<float_t, float>, double, float>;

        other_type other_val2(3.14);
        FloatViewModel<other_type> m4(&other_val2);
        REQUIRE_FALSE(m.are_equal(m4));

        FloatViewModel<const other_type> cm4(&other_val2);
        REQUIRE_FALSE(cm.are_equal(cm4));

        // Different const-ness
        REQUIRE_FALSE(m.are_equal(cm));
    }

    SECTION("to_string_") {
        if constexpr(wtf::concepts::StreamInsertable<float_t>) {
            std::stringstream ss;
            ss << val;
            REQUIRE(m.to_string() == ss.str());
            REQUIRE(cm.to_string() == ss.str());
        } else {
            REQUIRE(m.to_string() == "<unprintable float>");
            REQUIRE(cm.to_string() == "<unprintable float>");
        }
    }
}
