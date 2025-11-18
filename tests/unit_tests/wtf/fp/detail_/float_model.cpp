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
#include <wtf/fp/detail_/float_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::fp::detail_;

TEMPLATE_LIST_TEST_CASE("FloatModel", "[float_model]", test_wtf::all_fp_types) {
    using float_t = TestType;
    using model_t = FloatModel<float_t>;

    float_t val = 3.14;
    model_t m(val);

    SECTION("Ctors and assignment") {
        SECTION("Value ctor") {
            model_t m1(val);
            REQUIRE(m1.get_value() == val);
        }
        SECTION("Copy ctor") {
            model_t m2(m);
            REQUIRE(m2 == m);
            REQUIRE(m2.data() != m.data()); // Is deep?
        }
        SECTION("Copy assignment") {
            model_t m2(float_t(0));
            auto pm2 = &(m2 = m);
            REQUIRE(m2.get_value() == val);
            REQUIRE(m2.data() != m.data()); // Is deep?
            REQUIRE(pm2 == &m2);            // Returns *this
        }
        SECTION("Move ctor") {
            model_t m2(m);
            model_t m3(std::move(m2));
            REQUIRE(m3 == m);
        }
        SECTION("Move assignment") {
            model_t m2(float_t(0));
            model_t m3(m);
            auto pm2 = &(m2 = std::move(m3));
            REQUIRE(m2 == m);
            REQUIRE(pm2 == &m2); // Returns *this
        }
    }

    SECTION("get_value") { REQUIRE(m.get_value() == val); }

    SECTION("set_value") {
        float_t new_val = 1.23;
        m.set_value(new_val);
        REQUIRE(m.get_value() == new_val);
    }

    SECTION("data()") {
        auto p = m.data();
        REQUIRE(*p == val);
        *p = 2.71;
        REQUIRE(m.get_value() == float_t(2.71));
    }

    SECTION("data() const") {
        auto p = std::as_const(m).data();
        REQUIRE(*p == val);
    }

    SECTION("operator==") {
        model_t m2(val);
        REQUIRE(m == m2);

        model_t m3(float_t(1.23));
        REQUIRE_FALSE(m == m3);
    }

    SECTION("operator!=") {
        model_t m2(val);
        REQUIRE_FALSE(m != m2);

        model_t m3(float_t(1.23));
        REQUIRE(m != m3);
    }

    SECTION("swap") {
        model_t m2(float_t(1.23));
        m.swap(m2);
        REQUIRE(m.get_value() == float_t(1.23));
        REQUIRE(m2.get_value() == val);
    }

    SECTION("are_equal_") {
        model_t m2(val);
        REQUIRE(m.are_equal(m2));

        // Different value
        model_t m3(float_t(1.23));
        REQUIRE_FALSE(m.are_equal(m3));

        // Different type
        using other_type =
          std::conditional_t<std::is_same_v<float_t, float>, double, float>;

        FloatModel<other_type> m4(other_type(3.14));
        REQUIRE_FALSE(m.are_equal(m4));
    }
}
