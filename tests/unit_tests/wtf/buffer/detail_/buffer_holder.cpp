#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_model.hpp>

using namespace wtf::buffer::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("BufferHolder", "[wtf]", default_fp_types) {
    using model_type        = ContiguousModel<TestType>;
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = typename model_type::vector_type;

    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    model_type model(std::move(val));
    model_type empty(vector_type{});
    auto& holder       = model;
    auto& empty_holder = empty;

    SECTION("clone()") {
        REQUIRE(holder.clone()->are_equal(model));
        REQUIRE(empty_holder.clone()->are_equal(empty));
    }

    SECTION("at()") {
        REQUIRE(holder.at(0) == one);
        REQUIRE(holder.at(1) == two);
        REQUIRE(holder.at(2) == three);

        REQUIRE_THROWS_AS(holder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(empty_holder.at(0), std::out_of_range);

        // Can write to it
        holder.at(0) = TestType{4.0};
        REQUIRE(holder.at(0) == TestType{4.0});
    }

    SECTION("at() const") {
        const auto& cholder       = holder;
        const auto& cempty_holder = empty_holder;
        REQUIRE(cholder.at(0) == one);
        REQUIRE(cholder.at(1) == two);
        REQUIRE(cholder.at(2) == three);

        REQUIRE_THROWS_AS(cholder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cempty_holder.at(0), std::out_of_range);
    }

    SECTION("size()") {
        REQUIRE(holder.size() == 3);
        REQUIRE(empty_holder.size() == 0);
    }

    SECTION("type()") {
        REQUIRE(holder.type() == wtf::rtti::wtf_typeid<TestType>());
        REQUIRE(empty_holder.type() == wtf::rtti::wtf_typeid<TestType>());
    }

    SECTION("is_const()") {
        REQUIRE_FALSE(holder.is_const());
        REQUIRE_FALSE(empty_holder.is_const());
    }

    SECTION("is_contiguous()") {
        REQUIRE(holder.is_contiguous());
        REQUIRE(empty_holder.is_contiguous());
    }

    SECTION("are_equal()") {
        REQUIRE(holder.are_equal(model));
        REQUIRE(empty_holder.are_equal(empty));

        // Different sizes
        vector_type other_vector{1.0, 2.0};
        model_type different_size_model(std::move(other_vector));
        REQUIRE_FALSE(holder.are_equal(different_size_model));

        // Different values
        vector_type other_vector2{1.0, 2.0, 4.0};
        model_type different_values_model(std::move(other_vector2));
        REQUIRE_FALSE(holder.are_equal(different_values_model));

        // Different types
        using other_model_type = ContiguousModel<other_t>;
        std::vector<other_t> other_vector3{1.0, 2.0, 3.0};
        REQUIRE_FALSE(holder.are_equal(other_model_type(other_vector3)));
    }
}