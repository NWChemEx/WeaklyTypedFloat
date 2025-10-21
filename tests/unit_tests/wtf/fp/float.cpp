#include "../../../test_wtf.hpp"
#include <wtf/fp/float.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::fp;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("Float", "[wtf]", test_wtf::default_fp_types) {
    using float_t           = TestType;
    constexpr bool is_float = std::is_same_v<float_t, float>;
    using other_t           = std::conditional_t<is_float, double, float>;

    float_t val = 3.14;
    auto f      = make_float(val);

    SECTION("Ctors and assignment") {
        SECTION("make_float") {
            const float_t const_val = 1.23;
            auto cf                 = make_float(const_val);
            REQUIRE(float_cast<float_t>(f) == val);
            REQUIRE(float_cast<float_t>(cf) == const_val);
        }

        SECTION("by value") {
            Float f2 = val;
            Float f3(val);
            REQUIRE(f2 == f);
            REQUIRE(f3 == f);
        }

        SECTION("copy") {
            Float f2(f);
            REQUIRE(f2 == f);
        }

        SECTION("move") {
            Float f2(f);
            Float f3(std::move(f2));
            REQUIRE(f3 == f);
        }

        SECTION("copy assignment") {
            Float f2 = make_float(float_t(0));
            auto pf2 = &(f2 = f);
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);
        }

        SECTION("move assignment") {
            Float f2 = make_float(float_t(0));
            Float f3(f);
            auto pf2 = &(f2 = std::move(f3));
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);
        }
    }

    SECTION("Assign float") {
        // Can assign from the same floating point type
        f = float_t(1.23);
        REQUIRE(f == make_float(float_t(1.23)));

        // Can assign from a different floating point type
        f = other_t(3.14);
        REQUIRE(f == make_float(other_t(3.14)));
    }

    SECTION("swap") {
        Float f2 = make_float(float_t(0));
        f.swap(f2);
        REQUIRE(f == make_float(float_t(0)));
        REQUIRE(f2 == make_float(val));
    }

    SECTION("operator==") {
        REQUIRE(f == make_float(val));
        REQUIRE(f == val);
        REQUIRE(val == f);

        REQUIRE_FALSE(f == make_float(float_t(1.23)));
        REQUIRE_FALSE(f == float_t(1.23));
        REQUIRE_FALSE(float_t(1.23) == f);

        other_t other_val = 3.14;
        Float f2(other_val);
        REQUIRE_FALSE(f == f2);
    }

    SECTION("operator!=") {
        REQUIRE_FALSE(f != make_float(val));
        REQUIRE_FALSE(f != val);
        REQUIRE_FALSE(val != f);

        REQUIRE(f != make_float(float_t(1.23)));
        REQUIRE(f != float_t(1.23));
        REQUIRE(float_t(1.23) != f);

        other_t other_val = 3.14;
        Float f2(other_val);
        REQUIRE(f != f2);
    }
}

WTF_REGISTER_FP_TYPE(MyCustomFloat);

TEST_CASE("Float (custom type)", "[wtf][custom_type]") {
    using float_t = MyCustomFloat;
    using other_t = double;

    float_t val = 3.14;
    auto f      = make_float(val);

    SECTION("Ctors and assignment") {
        SECTION("make_float") {
            const float_t const_val = 1.23;
            auto cf                 = make_float(const_val);
            REQUIRE(float_cast<float_t>(f) == val);
            REQUIRE(float_cast<float_t>(cf) == const_val);
        }

        SECTION("by value") {
            Float f2 = val;
            Float f3(val);
            REQUIRE(f2 == f);
            REQUIRE(f3 == f);
        }

        SECTION("copy") {
            Float f2(f);
            REQUIRE(f2 == f);
        }

        SECTION("move") {
            Float f2(f);
            Float f3(std::move(f2));
            REQUIRE(f3 == f);
        }

        SECTION("copy assignment") {
            Float f2 = make_float(float_t(0));
            auto pf2 = &(f2 = f);
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);
        }

        SECTION("move assignment") {
            Float f2 = make_float(float_t(0));
            Float f3(f);
            auto pf2 = &(f2 = std::move(f3));
            REQUIRE(f2 == f);
            REQUIRE(pf2 == &f2);
        }
    }

    SECTION("Assign float") {
        // Can assign from the same floating point type
        f = float_t(1.23);
        REQUIRE(f == make_float(float_t(1.23)));

        // Can assign from a different floating point type
        f = other_t(3.14);
        REQUIRE(f == make_float(other_t(3.14)));
    }

    SECTION("swap") {
        Float f2 = make_float(float_t(0));
        f.swap(f2);
        REQUIRE(f == make_float(float_t(0)));
        REQUIRE(f2 == make_float(val));
    }

    SECTION("operator==") {
        REQUIRE(f == make_float(val));
        REQUIRE(f == val);
        REQUIRE(val == f);

        REQUIRE_FALSE(f == make_float(float_t(1.23)));
        REQUIRE_FALSE(f == float_t(1.23));
        REQUIRE_FALSE(float_t(1.23) == f);

        other_t other_val = 3.14;
        Float f2(other_val);
        REQUIRE_FALSE(f == f2);
    }

    SECTION("operator!=") {
        REQUIRE_FALSE(f != make_float(val));
        REQUIRE_FALSE(f != val);
        REQUIRE_FALSE(val != f);

        REQUIRE(f != make_float(float_t(1.23)));
        REQUIRE(f != float_t(1.23));
        REQUIRE(float_t(1.23) != f);

        other_t other_val = 3.14;
        Float f2(other_val);
        REQUIRE(f != f2);
    }
}

TEST_CASE("make_float", "[wtf]") {
    float val = 3.14f;
    auto f1   = make_float(val);
    REQUIRE(f1 == Float(3.14f));
    REQUIRE_FALSE(f1 == make_float<double>(val));
}

TEST_CASE("float_cast", "[wtf]") {
    float val = 3.14f;
    auto f    = make_float(val);
    REQUIRE(float_cast<float>(f) == val);

    // Can write to it
    float_cast<float&>(f) = 1.23f;
    REQUIRE(float_cast<float>(f) == 1.23f);

    REQUIRE_THROWS_AS(float_cast<double>(f), std::runtime_error);
}