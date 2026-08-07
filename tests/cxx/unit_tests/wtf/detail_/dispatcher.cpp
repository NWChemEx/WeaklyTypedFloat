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
 *   the address of the model provided to dispatch.
 * - We perfectly forward the callable. This is done using the ACallable struct.
 * - We can mix const and non-const models. This is tested by the "Testing the
 *   various cv-qualifications" section.
 */

/* Used to test perfect forwarding of a callable.
 *
 * In the test below we create an instance of this struct, set its m_address
 * member to the address of that instance, and then pass it to dispatch. If at
 * any point the callable is copied the value of the `this` pointer will
 * differ from m_address and the test will fail.
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

    SECTION("Three holders") {
        REQUIRE(dispatch<TypeModel, fp_types>(fxn, float_model, double_model,
                                              long_double_model) == 42);
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
