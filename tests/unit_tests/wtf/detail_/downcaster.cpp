#include "../../../test_wtf.hpp"
#include <wtf/detail_/downcaster.hpp>
#include <wtf/rtti/type_model.hpp>

using namespace wtf::rtti;
using namespace wtf::detail_;

using traits_type = VariantFromTuple<TypeModel, test_wtf::default_fp_types>;
using const_traits_type =
  ConstVariantFromTuple<TypeModel, test_wtf::default_fp_types>;

TEMPLATE_LIST_TEST_CASE("downcaster_impl", "", test_wtf::default_fp_types) {
    using value  = std::variant<TypeModel<TestType>*>;
    using cvalue = std::variant<TypeModel<const TestType>*>;

    TypeModel<TestType> model(typeid(TestType).name());
    TypeModel<const TestType> const_model(typeid(TestType).name());

    value corr{&model};
    cvalue corr_c{&const_model};

    REQUIRE(downcast_impl<0, value>(model) == corr);
    REQUIRE(downcast_impl<0, cvalue>(const_model) == corr_c);

    // Can't go const to non-const
    REQUIRE_THROWS_AS((downcast_impl<0, value>(const_model)),
                      std::runtime_error);

    // Throws if we run out of types
    REQUIRE_THROWS_AS((downcast_impl<1, value>(model)), std::runtime_error);
}

TEMPLATE_LIST_TEST_CASE("downcaster", "", test_wtf::default_fp_types) {
    using value_type = TypeModel<TestType>;

    using variant_type       = typename traits_type::value;
    using const_variant_type = typename const_traits_type::value;

    value_type model(typeid(TestType).name());
    const auto& cmodel = model;

    auto r = downcaster<TypeModel, test_wtf::default_fp_types>(model);
    REQUIRE(r == variant_type{&model});

    auto cr = downcaster<TypeModel, test_wtf::default_fp_types>(cmodel);
    REQUIRE(cr == const_variant_type{&cmodel});
}

TEMPLATE_LIST_TEST_CASE("const_downcaster", "", test_wtf::default_fp_types) {
    using value_type = TypeModel<const TestType>;

    using variant_type       = typename traits_type::cvalue;
    using const_variant_type = typename const_traits_type::cvalue;

    value_type model(typeid(TestType).name());
    const auto& cmodel = model;

    auto r = const_downcaster<TypeModel, test_wtf::default_fp_types>(model);
    REQUIRE(r == variant_type{&model});

    auto cr = const_downcaster<TypeModel, test_wtf::default_fp_types>(cmodel);
    REQUIRE(cr == const_variant_type{&cmodel});
}