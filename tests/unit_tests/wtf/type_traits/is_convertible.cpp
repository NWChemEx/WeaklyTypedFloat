#include "../../../test_wtf.hpp"
#include <wtf/type_traits/is_convertible.hpp>
#include <wtf/type_traits/type_traits.hpp>

template<typename FromType>
void check_conversions(std::array<bool, 3> expected) {
    using namespace wtf::type_traits;
    REQUIRE(IsConvertible<FromType, float>::value == expected[0]);
    REQUIRE(IsConvertible<FromType, double>::value == expected[1]);
    REQUIRE(IsConvertible<FromType, long double>::value == expected[2]);

    using wtf::type_traits::is_convertible_v;
    REQUIRE(is_convertible_v<FromType, float> == expected[0]);
    REQUIRE(is_convertible_v<FromType, double> == expected[1]);
    REQUIRE(is_convertible_v<FromType, long double> == expected[2]);
}

WTF_REGISTER_FP_TYPE(test_wtf::MyCustomFloat);

TEST_CASE("IsConvertible") {
    using namespace wtf::type_traits;
    check_conversions<float>({true, true, true});
    check_conversions<double>({true, true, true});
    check_conversions<long double>({true, true, true});
    check_conversions<test_wtf::MyCustomFloat>({false, false, false});
    REQUIRE(IsConvertible<double, test_wtf::MyCustomFloat>::value);
}