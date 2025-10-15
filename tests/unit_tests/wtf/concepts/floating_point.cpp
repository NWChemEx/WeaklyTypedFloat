#include "../../../test_wtf.hpp"
#include <wtf/concepts/floating_point.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("FloatingPoint", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(FloatingPoint<TestType>);
    STATIC_REQUIRE(FloatingPoint<const TestType>);
}