#include "../../../test_wtf.hpp"
#include <wtf/type_traits/is_floating_point.hpp>

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (true)", "[type_traits]",
                        test_wtf::all_fp_types) {
    STATIC_REQUIRE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE(wtf::type_traits::is_floating_point_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("IsFloatingPoint (false)", "[type_traits]",
                        test_wtf::not_fp_types) {
    STATIC_REQUIRE_FALSE(wtf::type_traits::IsFloatingPoint<TestType>::value);
    STATIC_REQUIRE_FALSE(wtf::type_traits::is_floating_point_v<TestType>);
}