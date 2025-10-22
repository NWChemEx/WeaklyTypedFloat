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