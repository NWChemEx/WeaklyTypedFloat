#include "../../../test_wtf.hpp"
#include <wtf/type_traits/is_floating_point.hpp>

namespace wtf::type_traits {

// Specialize IsFloatingPoint to recognize MyCustomFloat as a floating point
template<>
struct IsFloatingPoint<test_wtf::MyCustomFloat> {
    constexpr static bool value = true;
};

} // namespace wtf::type_traits

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (true)", "[type_traits]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE(wtf::type_traits::is_floating_point_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (false)", "[type_traits]",
                        test_wtf::not_fp_types) {
    STATIC_REQUIRE_FALSE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE_FALSE(wtf::type_traits::is_floating_point_v<TestType>);
}

TEST_CASE("IsFloatingPoint (custom true)", "[type_traits][custom_type]") {
    using test_wtf::MyCustomFloat;
    STATIC_REQUIRE(wtf::type_traits::IsFloatingPoint<MyCustomFloat>::value);
    STATIC_REQUIRE(wtf::type_traits::is_floating_point_v<MyCustomFloat>);
}