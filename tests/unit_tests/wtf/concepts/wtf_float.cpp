#include "../../../test_wtf.hpp"
#include <wtf/concepts/wtf_float.hpp>

using namespace wtf;

TEST_CASE("WTFFloat concept", "[wtf]") {
    STATIC_REQUIRE(wtf::concepts::WTFFloat<fp::Float>);
    STATIC_REQUIRE(wtf::concepts::WTFFloat<const fp::Float>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<float>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<double>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<long double>);
}