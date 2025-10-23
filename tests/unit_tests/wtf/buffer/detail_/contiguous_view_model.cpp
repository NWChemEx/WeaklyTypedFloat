#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_view_model.hpp>

using namespace wtf::buffer::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("ContiguousViewModel", "[wtf]", all_fp_types) {
    using model_type        = ContiguousViewModel<TestType>;
    using const_model_type  = ContiguousViewModel<const TestType>;
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = std::vector<TestType>;

    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    auto pdata = val.data();
    model_type model(pdata, val.size());
    const_model_type const_model(pdata, val.size());

    SECTION("Ctors") {
        SECTION("from pointer and size") {
            REQUIRE(model.get_element(0) == one);
            REQUIRE(model.get_element(1) == two);
            REQUIRE(model.get_element(2) == three);
            REQUIRE(&model.get_element(0) == pdata);

            REQUIRE(const_model.get_element(0) == one);
            REQUIRE(const_model.get_element(1) == two);
            REQUIRE(const_model.get_element(2) == three);
            REQUIRE(&const_model.get_element(0) == pdata);
        }

        SECTION("span") {
            model_type model_span(std::span<TestType>(pdata, val.size()));
            REQUIRE(model_span.get_element(0) == one);
            REQUIRE(model_span.get_element(1) == two);
            REQUIRE(model_span.get_element(2) == three);
            REQUIRE(&model_span.get_element(0) == pdata);

            const_model_type const_model_span(
              std::span<const TestType>(pdata, val.size()));
            REQUIRE(const_model_span.get_element(0) == one);
            REQUIRE(const_model_span.get_element(1) == two);
            REQUIRE(const_model_span.get_element(2) == three);
            REQUIRE(&const_model_span.get_element(0) == pdata);
        }

        SECTION("copy") {
            model_type model_copy(model);
            REQUIRE(model == model_copy);

            const_model_type const_model_copy(const_model);
            REQUIRE(const_model == const_model_copy);

            // Is shallow copy?
            REQUIRE(&model.get_element(0) == &model_copy.get_element(0));
            REQUIRE(&const_model.get_element(0) ==
                    &const_model_copy.get_element(0));
        }
    }

    SECTION("get_element()") {
        REQUIRE(model.get_element(0) == one);
        REQUIRE(model.get_element(1) == two);
        REQUIRE(model.get_element(2) == three);
        REQUIRE(&model.get_element(0) == pdata);
        REQUIRE_THROWS_AS(model.get_element(3), std::out_of_range);

        REQUIRE(const_model.get_element(0) == one);
        REQUIRE(const_model.get_element(1) == two);
        REQUIRE(const_model.get_element(2) == three);
        REQUIRE(&const_model.get_element(0) == pdata);
        REQUIRE_THROWS_AS(const_model.get_element(3), std::out_of_range);
    }

    SECTION("get_element() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.get_element(0) == one);
        REQUIRE(cmodel.get_element(1) == two);
        REQUIRE(cmodel.get_element(2) == three);
        REQUIRE(&cmodel.get_element(0) == pdata);
        REQUIRE_THROWS_AS(cmodel.get_element(3), std::out_of_range);

        const auto& cconst_model = const_model;
        REQUIRE(cconst_model.get_element(0) == one);
        REQUIRE(cconst_model.get_element(1) == two);
        REQUIRE(cconst_model.get_element(2) == three);
        REQUIRE(&cconst_model.get_element(0) == pdata);
        REQUIRE_THROWS_AS(cconst_model.get_element(3), std::out_of_range);
    }

    SECTION("data()") {
        REQUIRE(model.data() == pdata);
        REQUIRE(const_model.data() == pdata);
    }

    SECTION("data() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.data() == pdata);

        const auto& cconst_model = const_model;
        REQUIRE(cconst_model.data() == pdata);
    }

    SECTION("operator==") {
        REQUIRE(model == model_type(pdata, 3));
        REQUIRE(const_model == const_model_type(pdata, 3));

        // Different values
        vector_type diff_vector{one, one, one};
        REQUIRE_FALSE(model == model_type(diff_vector.data(), 3));
        REQUIRE_FALSE(const_model == const_model_type(diff_vector.data(), 3));

        // Different lengths
        REQUIRE_FALSE(model == model_type(pdata, 2));
        REQUIRE_FALSE(const_model == const_model_type(pdata, 2));

        // Different order
        vector_type reorder_vector{two, one, three};
        REQUIRE_FALSE(model == model_type(reorder_vector.data(), 3));
        REQUIRE_FALSE(const_model ==
                      const_model_type(reorder_vector.data(), 3));
    }

    SECTION("clone_()") {
        REQUIRE(model.clone()->are_equal(model));
        REQUIRE(const_model.clone()->are_equal(const_model));
    }

    SECTION("const_clone_()") {
        REQUIRE(model.const_clone()->are_equal(const_model));
        REQUIRE(const_model.const_clone()->are_equal(const_model));
    }

    SECTION("at_()") {
        REQUIRE(model.at(0) == one);
        REQUIRE(model.at(1) == two);
        REQUIRE(model.at(2) == three);

        REQUIRE(const_model.at(0) == one);
        REQUIRE(const_model.at(1) == two);
        REQUIRE(const_model.at(2) == three);
    }

    SECTION("at_() const") {
        const auto& cmodel = model;
        REQUIRE(cmodel.at(0) == one);
        REQUIRE(cmodel.at(1) == two);
        REQUIRE(cmodel.at(2) == three);

        const auto& cconst_model = const_model;
        REQUIRE(cconst_model.at(0) == one);
        REQUIRE(cconst_model.at(1) == two);
        REQUIRE(cconst_model.at(2) == three);
    }

    SECTION("size_()") {
        REQUIRE(model.size() == 3);
        REQUIRE(const_model.size() == 3);
    }

    SECTION("is_contiguous_()") {
        REQUIRE(model.is_contiguous());
        REQUIRE(const_model.is_contiguous());
    }

    SECTION("are_equal_") {
        model_type same_model(pdata, 3);
        REQUIRE(model.are_equal(same_model));

        const_model_type same_const_model(pdata, 3);
        REQUIRE(const_model.are_equal(same_const_model));

        // Different values
        vector_type diff_vector{one, one, one};
        model_type diff_model(diff_vector.data(), 3);
        REQUIRE_FALSE(model.are_equal(diff_model));

        const_model_type diff_const_model(diff_vector.data(), 3);
        REQUIRE_FALSE(const_model.are_equal(diff_const_model));

        // Different types
        using other_model_type = ContiguousViewModel<other_t>;
        std::vector<other_t> other_vector{1.0, 2.0, 3.0};
        other_model_type other_model(other_vector.data(), 3);
        REQUIRE_FALSE(model.are_equal(other_model));

        using const_other_model_type = ContiguousViewModel<const other_t>;
        const_other_model_type other_const_model(other_vector.data(), 3);
        REQUIRE_FALSE(const_model.are_equal(other_const_model));
    }
}