#include "../../../../test_wtf.hpp"
#include <wtf/fp/detail_/float_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace test_wtf;
using namespace wtf::fp;
WTF_REGISTER_FP_TYPE(MyCustomFloat);

TEMPLATE_LIST_TEST_CASE("FloatHolder", "[wtf]", default_fp_types) {
    using float_t = TestType;

    // Different type
    using other_type =
      std::conditional_t<std::is_same_v<float_t, float>, double, float>;

    using holder_t = wtf::fp::detail_::FloatHolder;
    using model_t  = wtf::fp::detail_::FloatModel<float_t>;

    float_t val = 3.14;
    model_t m(val);
    holder_t& h = m;

    SECTION("type()") { REQUIRE(h.type() == wtf::rtti::wtf_typeid<float_t>()); }

    SECTION("clone") { REQUIRE(h.clone()->are_equal(m)); }

    SECTION("change_value") {
        model_t m2(float_t(0));
        h.change_value(m2);
        REQUIRE(m.get_value() == float_t(0));

        model_t m3(float_t(1.23));
        h.change_value(m3);
        REQUIRE(m.get_value() == float_t(1.23));

        detail_::FloatModel<other_type> m4(other_type(3.14));
        REQUIRE_THROWS_AS(h.change_value(m4), std::invalid_argument);
    }

    SECTION("are_equal") {
        model_t m2(val);
        holder_t& h2 = m2;
        REQUIRE(h.are_equal(h2));

        model_t m3(float_t(1.23));
        holder_t& h3 = m3;
        REQUIRE_FALSE(h.are_equal(h3));

        detail_::FloatModel<other_type> m4(other_type(3.14));
        holder_t& h4 = m4;
        REQUIRE_FALSE(h.are_equal(h4));
    }
}

TEST_CASE("FloatHolder (custom type)", "[wtf][custom_type]") {
    using float_t = MyCustomFloat;

    // Different type
    using other_type = double;

    using holder_t = wtf::fp::detail_::FloatHolder;
    using model_t  = wtf::fp::detail_::FloatModel<float_t>;

    float_t val = 3.14;
    model_t m(val);
    holder_t& h = m;

    SECTION("type()") { REQUIRE(h.type() == wtf::rtti::wtf_typeid<float_t>()); }
    SECTION("clone") { REQUIRE(h.clone()->are_equal(m)); }

    SECTION("change_value") {
        model_t m2(float_t(0));
        h.change_value(m2);
        REQUIRE(m.get_value() == float_t(0));

        model_t m3(float_t(1.23));
        h.change_value(m3);
        REQUIRE(m.get_value() == float_t(1.23));

        detail_::FloatModel<other_type> m4(other_type(3.14));
        REQUIRE_THROWS_AS(h.change_value(m4), std::invalid_argument);
    }

    SECTION("are_equal") {
        model_t m2(val);
        holder_t& h2 = m2;
        REQUIRE(h.are_equal(h2));

        model_t m3(float_t(1.23));
        holder_t& h3 = m3;
        REQUIRE_FALSE(h.are_equal(h3));

        detail_::FloatModel<other_type> m4(other_type(3.14));
        holder_t& h4 = m4;
        REQUIRE_FALSE(h.are_equal(h4));
    }
}