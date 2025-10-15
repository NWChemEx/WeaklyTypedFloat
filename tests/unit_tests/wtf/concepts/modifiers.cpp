#include "../../../test_wtf.hpp"
#include <wtf/concepts/modifiers.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("ConstQualified", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(ConstQualified<TestType>);
    STATIC_REQUIRE(ConstQualified<const TestType>);
    STATIC_REQUIRE_FALSE(ConstQualified<volatile TestType>);
    STATIC_REQUIRE(ConstQualified<const volatile TestType>);
}

TEMPLATE_LIST_TEST_CASE("Unmodified", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(Unmodified<TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<TestType&>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType*>);
    STATIC_REQUIRE_FALSE(Unmodified<const TestType&>);
    STATIC_REQUIRE_FALSE(Unmodified<volatile TestType>);
    STATIC_REQUIRE_FALSE(Unmodified<const volatile TestType>);
}