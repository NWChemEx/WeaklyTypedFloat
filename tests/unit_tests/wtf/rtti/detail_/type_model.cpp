#include "../../../../test_wtf.hpp"
#include <wtf/rtti/detail_/type_model.hpp>
#include <wtf/type_traits/type_traits.hpp>

TEMPLATE_LIST_TEST_CASE("TypeModel", "[wtf][rtti]", test_wtf::all_fp_types) {
    using namespace wtf::rtti::detail_;

    std::string type_name = typeid(TestType).name();
    TypeModel<TestType> model(type_name);
    TypeModel<const TestType> const_model(type_name);

    SECTION("name_") {
        REQUIRE(model.name() == type_name);
        REQUIRE(const_model.name() == "const " + type_name);
    }

    SECTION("precision_") {
        using wtf::type_traits::precision_v;
        REQUIRE(model.precision() == precision_v<TestType>);
        REQUIRE(const_model.precision() == precision_v<TestType>);
    }

    SECTION("is_const_") {
        REQUIRE_FALSE(model.is_const());
        REQUIRE(const_model.is_const());
    }

    SECTION("make_const_") {
        REQUIRE(model.make_const()->are_equal(const_model));
        REQUIRE(const_model.make_const()->are_equal(const_model));
    }

    SECTION("are_equal_") {
        REQUIRE(model.are_equal(model));
        REQUIRE_FALSE(model.are_equal(const_model));

        REQUIRE_FALSE(const_model.are_equal(model));
        REQUIRE(const_model.are_equal(const_model));
    }
}
