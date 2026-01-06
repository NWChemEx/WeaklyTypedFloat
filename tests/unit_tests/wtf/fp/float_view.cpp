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
#include <wtf/fp/float.hpp>
#include <wtf/fp/float_view.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::fp;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("FloatView", "[wtf]", test_wtf::all_fp_types) {
    using float_t           = TestType;
    constexpr bool is_float = std::is_same_v<float_t, float>;
    using other_t           = std::conditional_t<is_float, double, float>;

    using view_type       = FloatView<Float>;
    using const_view_type = FloatView<const Float>;

    float_t val = 3.14;
    auto f      = make_float_view(val);
    auto cf     = make_float_view(std::as_const(val));

    SECTION("Ctors and assignment") {
        SECTION("make_float_view") {
            REQUIRE(&float_cast<float_t&>(f) == &val);
            REQUIRE(&float_cast<const float_t&>(cf) == &std::as_const(val));
        }

        SECTION("by value") {
            view_type f2 = val;
            view_type f3(val);
            REQUIRE(f2 == f);
            REQUIRE(f3 == f);

            const_view_type f4 = val;
            const_view_type f5(val);

            REQUIRE(f4 == cf);
            REQUIRE(f5 == cf);
        }

        SECTION("From Float object") {
            Float f_owning(val);
            view_type f2(f_owning);
            REQUIRE(f2 == f);
        }

        SECTION("non-const to const") {
            const_view_type f2 = f;
            const_view_type f3(f);
            REQUIRE(f2 == cf);
            REQUIRE(f3 == cf);
            REQUIRE(&float_cast<const float_t&>(f2) == &std::as_const(val));
            REQUIRE(&float_cast<const float_t&>(f3) == &std::as_const(val));
        }

        SECTION("copy") {
            view_type f2(f);
            REQUIRE(f2 == f);
            REQUIRE(&float_cast<float_t&>(f2) == &val);

            const_view_type f3(cf);
            REQUIRE(f3 == cf);
            REQUIRE(&float_cast<const float_t&>(f3) == &std::as_const(val));
        }

        SECTION("move") {
            view_type f2(f);
            view_type f3(std::move(f2));
            REQUIRE(f3 == f);
            REQUIRE(&float_cast<float_t&>(f3) == &val);

            const_view_type f4(cf);
            const_view_type f5(std::move(f4));
            REQUIRE(f5 == cf);
            REQUIRE(&float_cast<const float_t&>(f5) == &std::as_const(val));
        }

        SECTION("copy assignment") {
            float_t val2 = 0;
            auto f2      = make_float_view(val2);
            auto pf2     = &(f2 = f);
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);

            auto f3  = make_float_view(std::as_const(val2));
            auto pf3 = &(f3 = cf);
            REQUIRE(f3 == cf);
            REQUIRE(pf3 == &f3);
        }

        SECTION("move assignment") {
            float_t val2 = 0;
            auto f2      = make_float_view(val2);
            view_type f3(f);
            auto pf2 = &(f2 = std::move(f3));
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);

            auto f4 = make_float_view(std::as_const(val2));
            const_view_type f5(cf);
            auto pf4 = &(f4 = std::move(f5));
            REQUIRE(f4 == cf);
            REQUIRE(pf4 == &f4);
        }

        SECTION("Assign from float_t") {
            // Can assign from the same floating point type
            float_t val2(1.23);
            f = val2;
            REQUIRE(f == make_float_view(val2));
            REQUIRE(val == val2);
            REQUIRE(&float_cast<float_t&>(f) == &val);

            // Can't assign from a different floating point type
            other_t val3(3.14);
            REQUIRE_THROWS_AS(f = val3, std::runtime_error);
        }

        SECTION("Assign from Float") {
            float_t forty_two{42.0};
            auto f42 = wtf::fp::make_float(forty_two);
            auto pf  = &(f = f42);
            REQUIRE(f == forty_two);
            REQUIRE(pf == &f);

            other_t pi{3.14};
            auto fpi = wtf::fp::make_float(pi);
            REQUIRE_THROWS_AS(f = fpi, std::invalid_argument);
        }
    }

    SECTION("swap") {
        float_t val2 = 0;
        auto f2      = make_float_view(val2);
        f.swap(f2);
        REQUIRE(f == make_float_view(val2));
        REQUIRE(f2 == make_float_view(val));

        auto cf2 = make_float_view(std::as_const(val2));
        cf.swap(cf2);
        REQUIRE(cf == make_float_view(std::as_const(val2)));
        REQUIRE(cf2 == make_float_view(std::as_const(val)));
    }

    SECTION("operator==(FloatView)") {
        REQUIRE(f == make_float_view(val));
        REQUIRE(cf == make_float_view(std::as_const(val)));

        // Different values
        float_t other_val = 1.23;
        REQUIRE_FALSE(f == make_float_view(other_val));
        REQUIRE_FALSE(cf == make_float_view(std::as_const(other_val)));

        // Different types
        other_t other_val2 = 3.14;
        view_type f2(other_val2);
        REQUIRE_FALSE(f == f2);

        const_view_type cf2(other_val2);
        REQUIRE_FALSE(cf == cf2);

        // Different const-ness
        REQUIRE(f == cf);
    }

    SECTION("operator==(float_t)") {
        REQUIRE(f == val);
        REQUIRE(val == f);

        REQUIRE(cf == std::as_const(val));
        REQUIRE(std::as_const(val) == cf);

        // Different values
        float_t other_val = 1.23;
        REQUIRE_FALSE(f == other_val);
        REQUIRE_FALSE(other_val == f);

        REQUIRE_FALSE(cf == std::as_const(other_val));
        REQUIRE_FALSE(std::as_const(other_val) == cf);

        // Different types
        other_t other_val2 = 3.14;
        REQUIRE_FALSE(f == other_val2);
        REQUIRE_FALSE(other_val2 == f);

        REQUIRE_FALSE(cf == other_val2);
        REQUIRE_FALSE(other_val2 == cf);
    }

    SECTION("operator!=(FloatView)") {
        REQUIRE_FALSE(f != make_float_view(val));
        REQUIRE_FALSE(cf != make_float_view(std::as_const(val)));

        // Different values
        float_t other_val = 1.23;
        REQUIRE(f != make_float_view(other_val));
        REQUIRE(cf != make_float_view(std::as_const(other_val)));

        // Different types
        other_t other_val2 = 3.14;
        view_type f2(other_val2);
        REQUIRE(f != f2);

        const_view_type cf2(other_val2);
        REQUIRE(cf != cf2);

        // Different const-ness
        REQUIRE_FALSE(f != cf);
    }

    SECTION("operator!=(float_t)") {
        REQUIRE_FALSE(f != val);
        REQUIRE_FALSE(val != f);

        REQUIRE_FALSE(cf != std::as_const(val));
        REQUIRE_FALSE(std::as_const(val) != cf);

        // Different values
        float_t other_val = 1.23;
        REQUIRE(f != other_val);
        REQUIRE(other_val != f);

        REQUIRE(cf != std::as_const(other_val));
        REQUIRE(std::as_const(other_val) != cf);

        // Different types
        other_t other_val2 = 3.14;
        REQUIRE(f != other_val2);
        REQUIRE(other_val2 != f);

        REQUIRE(cf != other_val2);
        REQUIRE(other_val2 != cf);
    }

    SECTION("value()") {
        REQUIRE(f.template value<float_t>() == val);
        REQUIRE(&f.template value<float_t&>() == &val);
        REQUIRE(f.template value<const float_t>() == val);
        REQUIRE(&f.template value<const float_t&>() == &std::as_const(val));
        REQUIRE(cf.template value<const float_t>() == val);
        REQUIRE(&cf.template value<const float_t&>() == &std::as_const(val));

        REQUIRE_THROWS_AS(f.template value<other_t>(), std::runtime_error);
    }

    SECTION("value() const") {
        const auto& f_const  = f;
        const auto& cf_const = cf;
        REQUIRE(f_const.template value<float_t>() == val);
        REQUIRE(f_const.template value<const float_t>() == val);
        REQUIRE(&f_const.template value<const float_t&>() ==
                &std::as_const(val));

        REQUIRE(cf_const.template value<float_t>() == val);
        REQUIRE(cf_const.template value<const float_t>() == val);
        REQUIRE(&cf_const.template value<const float_t&>() ==
                &std::as_const(val));

        REQUIRE_THROWS_AS(f_const.template value<other_t>(),
                          std::runtime_error);
    }

    SECTION("to_string") {
        if constexpr(wtf::concepts::StreamInsertable<float_t>) {
            std::stringstream ss;
            ss << val;
            REQUIRE(f.to_string() == ss.str());
            REQUIRE(cf.to_string() == ss.str());
        } else {
            REQUIRE(f.to_string() == "<unprintable float>");
            REQUIRE(cf.to_string() == "<unprintable float>");
        }
    }
}

TEST_CASE("float_cast(FloatView)", "[wtf]") {
    float val = 3.14f;
    auto f    = make_float_view(val);
    auto cf   = make_float_view(std::as_const(val));

    // By value
    REQUIRE(float_cast<float>(f) == val);
    REQUIRE(float_cast<const float>(cf) == val);

    // By reference
    REQUIRE(&float_cast<float&>(f) == &val);
    REQUIRE(&float_cast<const float&>(cf) == &std::as_const(val));

    // Can write to non-const
    float_cast<float&>(f) = 1.23f;
    REQUIRE(val == 1.23f);

    // Can't use to convert
    REQUIRE_THROWS_AS(float_cast<double>(f), std::runtime_error);
    REQUIRE_THROWS_AS(float_cast<double&>(f), std::runtime_error);
}

TEST_CASE("make_float_view", "[wtf]") {
    float val = 3.14f;
    auto f1   = make_float_view(val);
    REQUIRE(&float_cast<float&>(f1) == &val);

    // Properly deduces const-ness
    auto cf1 = make_float_view(std::as_const(val));
    STATIC_REQUIRE(std::is_same_v<decltype(cf1), FloatView<const Float>>);
    REQUIRE(&float_cast<const float&>(cf1) == &std::as_const(val));

    // Can explicitly specify const-ness
    auto cf2 = make_float_view<const float>(val);
    REQUIRE(&float_cast<const float&>(cf2) == &std::as_const(val));
}
