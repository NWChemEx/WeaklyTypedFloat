#include "../../../test_wtf.hpp"
#include <wtf/rtti/type_model.hpp>

using namespace wtf::rtti;

using TypeModelf = wtf::rtti::TypeModel<float>;
using TypeModeld = wtf::rtti::TypeModel<double>;
using TypeModell = wtf::rtti::TypeModel<long double>;

TEMPLATE_LIST_TEST_CASE("TypeHolder", "[wtf][rtti]",
                        test_wtf::default_fp_types) {
    using namespace wtf::rtti;
    std::string type_name = typeid(TestType).name();
    TypeModel<TestType> model(type_name);
    TypeModel<const TestType> const_model(type_name);

    TypeHolder& model_holder       = model;
    TypeHolder& const_model_holder = const_model;

    SECTION("get_model") {
        auto m1 = model_holder.get_model();
        auto m2 = const_model_holder.get_model();

        REQUIRE(m1->are_equal(model));
        REQUIRE(m2->are_equal(const_model));

        // Must return the SAME pointer each time
        REQUIRE(m1 == model_holder.get_model());
        REQUIRE(m2 == const_model_holder.get_model());
    }

    SECTION("is_implicitly_convertible_to") {
        using types = test_wtf::default_fp_types;
        REQUIRE(model_holder.is_implicitly_convertible_to<types>(model));
        REQUIRE(model_holder.is_implicitly_convertible_to<types>(const_model));

        REQUIRE(
          const_model_holder.is_implicitly_convertible_to<types>(const_model));
        REQUIRE_FALSE(
          const_model_holder.is_implicitly_convertible_to<types>(model));
    }
}