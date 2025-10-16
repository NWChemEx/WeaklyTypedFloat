#include "../../../test_wtf.hpp"
#include <wtf/type_traits/precision.hpp>
#include <wtf/type_traits/type_traits.hpp>

WTF_REGISTER_FP_TYPE(test_wtf::MyCustomFloat);

using namespace wtf::type_traits;

TEST_CASE("Precision") {
    SECTION("float") {
        STATIC_REQUIRE(Precision<float>::value == 6);
        STATIC_REQUIRE(precision_v<float> == 6);
    }
    SECTION("double") {
        STATIC_REQUIRE(Precision<double>::value == 15);
        STATIC_REQUIRE(precision_v<double> == 15);
    }
    SECTION("long double") {
        STATIC_REQUIRE(Precision<long double>::value >= 15);
        STATIC_REQUIRE(precision_v<long double> >= 15);
    }
    SECTION("MyCustomFloat") {
        STATIC_REQUIRE(Precision<test_wtf::MyCustomFloat>::value == 0);
        STATIC_REQUIRE(precision_v<test_wtf::MyCustomFloat> == 0);
    }
}