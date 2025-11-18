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
#include <wtf/detail_/dispatcher.hpp>
#include <wtf/rtti/detail_/type_model.hpp>

using namespace wtf::detail_;
using wtf::rtti::detail_::TypeModel;
namespace {

using fmodel_type  = TypeModel<float>;
using dmodel_type  = TypeModel<const double>;
using ldmodel_type = TypeModel<long double>;

} // namespace

/* Notes on testing.
 *
 * For performance reasons it must be the case that:
 * - Holders can be forwarded into the callable without copies. Tested by
 *   ensuring the address of the model passed to the callable is the same as
 *   the address of the model provided to the DispatchHelper.
 * - We perfectly forward the callable. This is done using the ACallable struct.
 * - We can mix const and non-const models. This is tested by the double_lambda.
 */

/* Used to test perfect forwarding of a callable.
 *
 * In the test below we create an instance of this struct, set its m_address
 * member to the address of that instance, and then pass it to the
 * DispatchHelper's run method. If at any point the callable is copied the
 * value of the `this` pointer will differ from m_address and the test will
 * fail.
 */
struct ACallable {
    template<typename T>
    int operator()(const T& m) {
        REQUIRE(m_address == this);
        return 42;
    }
    ACallable* m_address;
};

struct Callable {
    Callable(fmodel_type* pfloat_model_in) :
      pfloat_model(pfloat_model_in),
      float_model("float"),
      double_model("double"),
      long_double_model("long double") {}

    template<typename T>
    int operator()(const T& m) {
        if constexpr(std::is_same_v<T, fmodel_type>) {
            REQUIRE(&m == pfloat_model);
        }
        REQUIRE(m.are_equal(float_model));
        return 42;
    }

    template<typename T1, typename T2>
    int operator()(T1&& m1, T2&& m2) {
        REQUIRE(m1.are_equal(float_model));
        REQUIRE(m2.are_equal(double_model));
        return 42;
    }

    template<typename T1, typename T2, typename T3>
    int operator()(const T1& m1, const T2& m2, const T3& m3) {
        REQUIRE(m1.are_equal(float_model));
        REQUIRE(m2.are_equal(double_model));
        REQUIRE(m3.are_equal(long_double_model));
        return 42;
    }

    fmodel_type* pfloat_model;
    fmodel_type float_model;
    dmodel_type double_model;
    ldmodel_type long_double_model;
};

TEST_CASE("DispatchHelper") {
    fmodel_type float_model("float");
    dmodel_type double_model("double");
    ldmodel_type long_double_model("long double");
    auto pfloat_model = &float_model;

    using float_variant       = std::variant<fmodel_type*>;
    using double_variant      = std::variant<dmodel_type*>;
    using long_double_variant = std::variant<ldmodel_type*>;

    float_variant fvariant{&float_model};
    double_variant dvariant{&double_model};
    long_double_variant ldvariant{&long_double_model};

    auto float_lambda = [=](const fmodel_type& m) {
        REQUIRE(&m == pfloat_model);
        return 42;
    };

    auto double_lambda = [=](fmodel_type& fm, const dmodel_type& dm) {
        REQUIRE(&fm == pfloat_model);
        REQUIRE(dm.are_equal(double_model));
        return 42;
    };

    auto long_double_lambda = [=](const fmodel_type& fm, const dmodel_type& dm,
                                  const ldmodel_type& ldm) {
        REQUIRE(&fm == pfloat_model);
        REQUIRE(dm.are_equal(double_model));
        REQUIRE(ldm.are_equal(long_double_model));
        return 42;
    };

    // Sanity check the lambdas work
    REQUIRE(float_lambda(float_model) == 42);
    REQUIRE(double_lambda(float_model, double_model) == 42);
    REQUIRE(long_double_lambda(float_model, double_model, long_double_model) ==
            42);

    SECTION("Zero variants") {
        auto fxn = []() { return 42; };

        using variant_tuple = std::tuple<>;
        using helper_type   = DispatchHelper<variant_tuple>;
        helper_type helper(variant_tuple{});
        REQUIRE(helper.run(fxn) == 42);

        auto helper2            = helper.add_variant(fvariant);
        using new_variant_tuple = std::tuple<float_variant>;
        using old_helper        = decltype(helper2);
        using corr_helper       = DispatchHelper<new_variant_tuple>;
        STATIC_REQUIRE(std::is_same_v<old_helper, corr_helper>);
        REQUIRE(helper2.run(float_lambda) == 42);
    }

    SECTION("One variant") {
        using variant_tuple = std::tuple<float_variant>;
        using helper_type   = DispatchHelper<variant_tuple>;
        helper_type helper(variant_tuple{fvariant});
        REQUIRE(helper.run(float_lambda) == 42);

        // Check that acallable is perfectly forwarded
        ACallable acallable;
        acallable.m_address = &acallable;
        REQUIRE(helper.run(acallable) == 42);

        auto helper2            = helper.add_variant(dvariant);
        using new_variant_tuple = std::tuple<float_variant, double_variant>;
        using old_helper        = decltype(helper2);
        using corr_helper       = DispatchHelper<new_variant_tuple>;
        STATIC_REQUIRE(std::is_same_v<old_helper, corr_helper>);
        REQUIRE(helper2.run(double_lambda) == 42);
    }

    SECTION("Two variants") {
        using variant_tuple = std::tuple<float_variant, double_variant>;
        using helper_type   = DispatchHelper<variant_tuple>;
        helper_type helper(variant_tuple{fvariant, dvariant});
        REQUIRE(helper.run(double_lambda) == 42);

        auto helper2 = helper.add_variant(ldvariant);
        using new_variant_tuple =
          std::tuple<float_variant, double_variant, long_double_variant>;
        using old_helper  = decltype(helper2);
        using corr_helper = DispatchHelper<new_variant_tuple>;
        STATIC_REQUIRE(std::is_same_v<old_helper, corr_helper>);
        REQUIRE(helper2.run(long_double_lambda) == 42);
    }
}

TEST_CASE("dispatch") {
    using fp_types = test_wtf::default_fp_types;
    fmodel_type float_model("float");
    dmodel_type double_model("double");
    ldmodel_type long_double_model("long double");
    auto pfloat_model = &float_model;

    Callable fxn(pfloat_model);

    SECTION("Zero holders") {
        // Zero idea why we would ever do this, but...it should work so we test
        // it.
        auto zero_fxn = []() { return 42; };
        REQUIRE(dispatch<TypeModel, fp_types>(zero_fxn) == 42);
    }

    SECTION("One holder") {
        REQUIRE(dispatch<TypeModel, fp_types>(fxn, float_model) == 42);

        // Check that acallable is perfectly forwarded
        ACallable acallable;
        acallable.m_address = &acallable;
        REQUIRE(dispatch<TypeModel, fp_types>(acallable, float_model) == 42);
    }

    SECTION("Two holders") {
        REQUIRE(dispatch<TypeModel, fp_types>(fxn, float_model, double_model) ==
                42);
    }

    SECTION("Testing the various cv-qualifications") {
        auto lambda = [](auto&& fm, auto&& dm) { return 42; };

        TypeModel<float> fm0("float");
        TypeModel<const float> fm1("float");

        REQUIRE(dispatch<TypeModel, fp_types>(lambda, fm0, fm0) == 42);
        REQUIRE(dispatch<TypeModel, fp_types>(lambda, fm0, fm1) == 42);
        REQUIRE(dispatch<TypeModel, fp_types>(lambda, fm1, fm0) == 42);
        REQUIRE(dispatch<TypeModel, fp_types>(lambda, fm1, fm1) == 42);
    }
}
