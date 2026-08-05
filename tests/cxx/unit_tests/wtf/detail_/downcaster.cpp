/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../../test_wtf.hpp"
#include <wtf/detail_/downcaster.hpp>
#include <wtf/rtti/detail_/type_model.hpp>

using namespace wtf::detail_;
using wtf::rtti::detail_::TypeModel;

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
