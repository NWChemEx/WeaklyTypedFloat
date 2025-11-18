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

#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_model.hpp>

using namespace wtf::buffer::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("ContiguousModel", "[wtf]", all_fp_types) {
    using model_type        = ContiguousModel<TestType>;
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = typename model_type::vector_type;

    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    auto pdata = val.data();
    model_type model(std::move(val));

    SECTION("Ctors") {
        SECTION("vector") {
            REQUIRE(model.get_element(0) == one);
            REQUIRE(model.get_element(1) == two);
            REQUIRE(model.get_element(2) == three);
            REQUIRE(&model.get_element(0) == pdata);
        }

        SECTION("copy") {
            model_type model_copy(model);
            REQUIRE(model == model_copy);

            // Is deep copy?
            REQUIRE_FALSE(&model.get_element(0) == &model_copy.get_element(0));
        }
    }

    SECTION("get_element()") {
        REQUIRE(model.get_element(0) == one);
        REQUIRE(model.get_element(1) == two);
        REQUIRE(model.get_element(2) == three);

        REQUIRE_THROWS_AS(model.get_element(3), std::out_of_range);
    }

    SECTION("get_element() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.get_element(0) == one);
        REQUIRE(cmodel.get_element(1) == two);
        REQUIRE(cmodel.get_element(2) == three);
        REQUIRE_THROWS_AS(cmodel.get_element(3), std::out_of_range);
    }

    SECTION("data()") { REQUIRE(model.data() == pdata); }

    SECTION("data() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.data() == pdata);
    }

    SECTION("span()") {
        auto span = model.span();
        REQUIRE(span.data() == pdata);
        REQUIRE(span.size() == 3);
    }

    SECTION("span() const") {
        const auto& cmodel = model;
        auto span          = cmodel.span();
        REQUIRE(span.data() == pdata);
        REQUIRE(span.size() == 3);
    }

    SECTION("operator==") {
        REQUIRE(model == model_type(vector_type{one, two, three}));

        // Different values
        REQUIRE_FALSE(model == model_type(vector_type{one, one, one}));

        // Different lengths
        REQUIRE_FALSE(model == model_type(vector_type{one, two}));

        // Different order
        REQUIRE_FALSE(model == model_type(vector_type{two, one, three}));
    }

    SECTION("clone_()") { REQUIRE(model.clone()->are_equal(model)); }

    SECTION("at_()") {
        REQUIRE(model.at(0) == one);
        REQUIRE(model.at(1) == two);
        REQUIRE(model.at(2) == three);
    }

    SECTION("at_() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.at(0) == one);
        REQUIRE(cmodel.at(1) == two);
        REQUIRE(cmodel.at(2) == three);
    }

    SECTION("size_()") { REQUIRE(model.size() == 3); }

    SECTION("is_const_()") { REQUIRE_FALSE(model.is_const()); }

    SECTION("is_contiguous_()") { REQUIRE(model.is_contiguous()); }

    SECTION("are_equal_") {
        model_type same_model(vector_type{one, two, three});
        REQUIRE(model.are_equal(same_model));

        // Different values
        model_type diff_model(vector_type{one, one, one});
        REQUIRE_FALSE(model.are_equal(diff_model));

        // Different types
        using other_model_type = ContiguousModel<other_t>;
        std::vector<other_t> other_vector{1.0, 2.0, 3.0};
        REQUIRE_FALSE(model.are_equal(other_model_type(other_vector)));
    }
}

struct CheckVisitContiguousModel {
    CheckVisitContiguousModel(float* pdataf, double* pdatad) :
      pdataf_corr(pdataf), pdatad_corr(pdatad) {}

    auto operator()(std::span<const float> span) const {
        REQUIRE(span.data() == pdataf_corr);
        REQUIRE(span.size() == 3);
    }

    auto operator()(std::span<const double> span) const {
        REQUIRE(span.data() == pdatad_corr);
        REQUIRE(span.size() == 3);
    }

    auto operator()(std::span<float> lhs, std::span<double> rhs) const {
        REQUIRE(lhs.data() == pdataf_corr);
        REQUIRE(lhs.size() == 3);
        REQUIRE(rhs.data() == pdatad_corr);
        REQUIRE(rhs.size() == 3);
    }

    template<typename T, typename U>
    auto operator()(std::span<T> lhs, std::span<U> rhs) const {
        throw std::runtime_error("Only float, double supported");
    }

    float* pdataf_corr;
    double* pdatad_corr;
};

TEST_CASE("visit_contiguous_model") {
    std::vector<float> valf{1.0, 2.0, 3.0};
    std::vector<double> vald{1.0, 2.0, 3.0};
    auto pdataf = valf.data();
    auto pdatad = vald.data();

    CheckVisitContiguousModel visitor(pdataf, pdatad);

    ContiguousModel<float> modelf(std::move(valf));
    ContiguousModel<double> modeld(std::move(vald));

    using type_tuple = std::tuple<float, double>;

    SECTION("one argument") {
        visit_contiguous_model<type_tuple>(visitor, modelf);
        visit_contiguous_model<type_tuple>(visitor, modeld);
    }

    SECTION("Two arguments") {
        visit_contiguous_model<type_tuple>(visitor, modelf, modeld);
    }
}
