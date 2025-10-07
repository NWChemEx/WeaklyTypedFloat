#include "../../../test_wtf.hpp"
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/type_traits.hpp>

// The need for "typename" makes these pretty long. To clean up the test we
// define a macro for aliasing the members of the traits class.
#define UNPACK_TRAITS(traits)                                 \
    using value_type      = typename traits::value_type;      \
    using reference       = typename traits::reference;       \
    using const_reference = typename traits::const_reference; \
    using pointer         = typename traits::pointer;         \
    using const_pointer   = typename traits::const_pointer

TEMPLATE_LIST_TEST_CASE("float_traits", "[type_traits]",
                        test_wtf::default_fp_types) {
    SECTION("unqualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
    }

    SECTION("const-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<const TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, const TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, const TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
    }

    // Correct type for a cv reference type (factored out to it fit on one line)
    using cv_reference = const volatile TestType&;

    SECTION("volatile-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, volatile TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<pointer, volatile TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const volatile TestType*>);
    }

    SECTION("cv-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<const volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, const volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<const_reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<pointer, const volatile TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const volatile TestType*>);
    }
}

WTF_REGISTER_FP_TYPE(test_wtf::MyCustomFloat);

TEST_CASE("float_traits (custom type)", "[type_traits][custom_type]") {
    using TestType = test_wtf::MyCustomFloat;
    SECTION("unqualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
    }

    SECTION("const-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<const TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, const TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const TestType&>);
        STATIC_REQUIRE(std::is_same_v<pointer, const TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const TestType*>);
    }

    // Correct type for a cv reference type (factored out to it fit on one line)
    using cv_reference = const volatile TestType&;

    SECTION("volatile-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, volatile TestType&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<pointer, volatile TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const volatile TestType*>);
    }

    SECTION("cv-qualified type") {
        UNPACK_TRAITS(wtf::type_traits::float_traits<const volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<value_type, const volatile TestType>);
        STATIC_REQUIRE(std::is_same_v<reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<const_reference, cv_reference>);
        STATIC_REQUIRE(std::is_same_v<pointer, const volatile TestType*>);
        STATIC_REQUIRE(std::is_same_v<const_pointer, const volatile TestType*>);
    }
}

#undef UNPACK_TRAITS