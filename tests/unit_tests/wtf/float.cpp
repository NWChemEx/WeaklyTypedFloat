#include "../../test_wtf.hpp"
#include <wtf/float.hpp>

using namespace wtf;

TEMPLATE_LIST_TEST_CASE("make_float", "[float]", test_wtf::default_fp_types) {
    using float_t = TestType;

    float_t val = 3.14;
    auto f      = make_float(val);

    SECTION("By value") { REQUIRE(float_cast<float_t>(f) == val); }

    // SECTION("By reference") {
    //     float_cast<float_t&>(f) = 1.23;
    //     REQUIRE(float_cast<float_t&>(f) == float_t{1.23});
    // }

    // SECTION("By const reference") {
    //     REQUIRE(float_cast<const float_t&>(f) == val);
    // }
}