#include "../../../../test_wtf.hpp"
#include <wtf/fp/detail_/float_view_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace test_wtf;
using namespace wtf::fp;
WTF_REGISTER_FP_TYPE(MyCustomFloat);

TEMPLATE_LIST_TEST_CASE("FloatViewHolder", "[wtf]", default_fp_types) {
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
}

TEST_CASE("FloatViewHolder (custom type)", "[wtf][custom_type]") {
    using float_t = MyCustomFloat;

    // Different type
    using other_type = double;

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
}