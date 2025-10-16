#include "../../../test_wtf.hpp"
#include <wtf/concepts/floating_point.hpp>
#include <wtf/type_traits/type_traits.hpp>

WTF_REGISTER_FP_TYPE(test_wtf::MyCustomFloat);

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("UnmodifiedFloatingPoint", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(UnmodifiedFloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile TestType&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile TestType&>);
}

TEST_CASE("UnmodifiedFloatingPoint (custom type)",
          "[wtf][concepts][custom_type]") {
    using test_wtf::MyCustomFloat;
    STATIC_REQUIRE(UnmodifiedFloatingPoint<MyCustomFloat>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<MyCustomFloat&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const MyCustomFloat>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const MyCustomFloat&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<volatile MyCustomFloat&>);
    STATIC_REQUIRE_FALSE(UnmodifiedFloatingPoint<const volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(
      UnmodifiedFloatingPoint<const volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(
      UnmodifiedFloatingPoint<const volatile MyCustomFloat&>);
}

TEMPLATE_LIST_TEST_CASE("FloatingPoint", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE(FloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<TestType&>);
    STATIC_REQUIRE(FloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const TestType&>);
    STATIC_REQUIRE(FloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile TestType&>);
    STATIC_REQUIRE(FloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile TestType&>);
}

TEST_CASE("FloatingPoint (custom type)", "[wtf][concepts][custom_type]") {
    using test_wtf::MyCustomFloat;
    STATIC_REQUIRE(FloatingPoint<MyCustomFloat>);
    STATIC_REQUIRE_FALSE(FloatingPoint<MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<MyCustomFloat&>);
    STATIC_REQUIRE(FloatingPoint<const MyCustomFloat>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const MyCustomFloat&>);
    STATIC_REQUIRE(FloatingPoint<volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<volatile MyCustomFloat&>);
    STATIC_REQUIRE(FloatingPoint<const volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(FloatingPoint<const volatile MyCustomFloat&>);
}

TEMPLATE_LIST_TEST_CASE("ConstFloatingPoint", "[wtf][concepts]",
                        test_wtf::default_fp_types) {
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<TestType&>);
    STATIC_REQUIRE(ConstFloatingPoint<const TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const TestType&>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile TestType&>);
    STATIC_REQUIRE(ConstFloatingPoint<const volatile TestType>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile TestType*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile TestType&>);
}

TEST_CASE("ConstFloatingPoint (custom type)", "[wtf][concepts][custom_type]") {
    using test_wtf::MyCustomFloat;
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<MyCustomFloat>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<MyCustomFloat&>);
    STATIC_REQUIRE(ConstFloatingPoint<const MyCustomFloat>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const MyCustomFloat&>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<volatile MyCustomFloat&>);
    STATIC_REQUIRE(ConstFloatingPoint<const volatile MyCustomFloat>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile MyCustomFloat*>);
    STATIC_REQUIRE_FALSE(ConstFloatingPoint<const volatile MyCustomFloat&>);
}