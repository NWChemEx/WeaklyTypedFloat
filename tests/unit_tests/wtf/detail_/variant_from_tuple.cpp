#include "../../../test_wtf.hpp"
#include <wtf/detail_/variant_from_tuple.hpp>

namespace {

template<wtf::concepts::FloatingPoint T>
struct Model;

}

using f  = Model<float>;
using cf = Model<const float>;
using d  = Model<double>;
using cd = Model<const double>;
using l  = Model<long double>;
using cl = Model<const long double>;

TEST_CASE("VariantFromTuple") {
    SECTION("zero types") {
        using tuple   = std::tuple<>;
        using variant = wtf::detail_::VariantFromTuple<Model, tuple>;
        STATIC_REQUIRE(std::is_same_v<variant::value, std::variant<>>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, std::variant<>>);
    }
    SECTION("one type") {
        using tuple   = std::tuple<float>;
        using variant = wtf::detail_::VariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<f*>;
        using cvalue_corr = std::variant<cf*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
    SECTION("two types") {
        using tuple   = std::tuple<float, double>;
        using variant = wtf::detail_::VariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<f*, d*>;
        using cvalue_corr = std::variant<cf*, cd*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
    SECTION("default types") {
        using tuple   = wtf::default_fp_types;
        using variant = wtf::detail_::VariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<f*, d*, l*>;
        using cvalue_corr = std::variant<cf*, cd*, cl*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
}

TEST_CASE("ConstVariantFromTuple") {
    SECTION("zero types") {
        using tuple   = std::tuple<>;
        using variant = wtf::detail_::ConstVariantFromTuple<Model, tuple>;
        STATIC_REQUIRE(std::is_same_v<variant::value, std::variant<>>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, std::variant<>>);
    }
    SECTION("one type") {
        using tuple   = std::tuple<float>;
        using variant = wtf::detail_::ConstVariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<const f*>;
        using cvalue_corr = std::variant<const cf*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
    SECTION("two types") {
        using tuple   = std::tuple<float, double>;
        using variant = wtf::detail_::ConstVariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<const f*, const d*>;
        using cvalue_corr = std::variant<const cf*, const cd*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
    SECTION("default types") {
        using tuple   = wtf::default_fp_types;
        using variant = wtf::detail_::ConstVariantFromTuple<Model, tuple>;

        using value_corr  = std::variant<const f*, const d*, const l*>;
        using cvalue_corr = std::variant<const cf*, const cd*, const cl*>;

        STATIC_REQUIRE(std::is_same_v<variant::value, value_corr>);
        STATIC_REQUIRE(std::is_same_v<variant::cvalue, cvalue_corr>);
    }
}