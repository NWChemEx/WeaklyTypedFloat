#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_view_model.hpp>

using namespace wtf::buffer::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("BufferViewHolder", "[wtf]", default_fp_types) {
    using model_type       = ContiguousViewModel<TestType>;
    using const_model_type = ContiguousViewModel<const TestType>;

    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = std::vector<TestType>;

    using holder_type       = BufferViewHolder<wtf::fp::Float>;
    using const_holder_type = BufferViewHolder<const wtf::fp::Float>;

    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};

    model_type model(val.data(), 3);
    model_type empty(nullptr, 0);
    const_model_type const_model(val.data(), 3);
    const_model_type const_empty(nullptr, 0);

    holder_type& holder                   = model;
    holder_type& empty_holder             = empty;
    const_holder_type& const_holder       = const_model;
    const_holder_type& const_empty_holder = const_empty;

    SECTION("clone()") {
        REQUIRE(holder.clone()->are_equal(model));
        REQUIRE(empty_holder.clone()->are_equal(empty));

        REQUIRE(const_holder.clone()->are_equal(const_model));
        REQUIRE(const_empty_holder.clone()->are_equal(const_empty));
    }

    SECTION("const_clone()") {
        REQUIRE(holder.const_clone()->are_equal(const_model));
        REQUIRE(empty_holder.const_clone()->are_equal(const_empty));
        REQUIRE(const_holder.const_clone()->are_equal(const_model));
        REQUIRE(const_empty_holder.const_clone()->are_equal(const_empty));
    }

    SECTION("at()") {
        REQUIRE(holder.at(0) == one);
        REQUIRE(holder.at(1) == two);
        REQUIRE(holder.at(2) == three);

        REQUIRE(const_holder.at(0) == one);
        REQUIRE(const_holder.at(1) == two);
        REQUIRE(const_holder.at(2) == three);

        REQUIRE_THROWS_AS(holder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(empty_holder.at(0), std::out_of_range);

        REQUIRE_THROWS_AS(const_holder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(const_empty_holder.at(0), std::out_of_range);

        // Can write to mutable holder
        holder.at(0) = TestType{4.0};
        REQUIRE(holder.at(0) == TestType{4.0});
    }

    SECTION("at() const") {
        const auto& cholder             = holder;
        const auto& cempty_holder       = empty_holder;
        const auto& cconst_holder       = const_holder;
        const auto& cconst_empty_holder = const_empty_holder;

        REQUIRE(cholder.at(0) == one);
        REQUIRE(cholder.at(1) == two);
        REQUIRE(cholder.at(2) == three);

        REQUIRE(cconst_holder.at(0) == one);
        REQUIRE(cconst_holder.at(1) == two);
        REQUIRE(cconst_holder.at(2) == three);

        REQUIRE_THROWS_AS(cholder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cempty_holder.at(0), std::out_of_range);

        REQUIRE_THROWS_AS(cconst_holder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cconst_empty_holder.at(0), std::out_of_range);
    }

    SECTION("size()") {
        REQUIRE(holder.size() == 3);
        REQUIRE(empty_holder.size() == 0);

        REQUIRE(const_holder.size() == 3);
        REQUIRE(const_empty_holder.size() == 0);
    }

    SECTION("type()") {
        REQUIRE(holder.type() == wtf::rtti::wtf_typeid<TestType>());
        REQUIRE(empty_holder.type() == wtf::rtti::wtf_typeid<TestType>());

        auto corr_const_type = wtf::rtti::wtf_typeid<const TestType>();
        REQUIRE(const_holder.type() == corr_const_type);
        REQUIRE(const_empty_holder.type() == corr_const_type);
    }

    SECTION("is_contiguous()") {
        REQUIRE(holder.is_contiguous());
        REQUIRE(empty_holder.is_contiguous());

        REQUIRE(const_holder.is_contiguous());
        REQUIRE(const_empty_holder.is_contiguous());
    }

    SECTION("are_equal()") {
        REQUIRE(holder.are_equal(model));
        REQUIRE(empty_holder.are_equal(empty));

        REQUIRE(const_holder.are_equal(const_model));
        REQUIRE(const_empty_holder.are_equal(const_empty));

        // Different sizes
        vector_type other_vector{1.0, 2.0};
        model_type different_size_model(other_vector.data(), 2);
        REQUIRE_FALSE(holder.are_equal(different_size_model));

        const_model_type different_size_const_model(other_vector.data(), 2);
        REQUIRE_FALSE(const_holder.are_equal(different_size_const_model));

        // Different values
        vector_type other_vector2{1.0, 2.0, 4.0};
        model_type different_values_model(other_vector2.data(), 3);
        REQUIRE_FALSE(holder.are_equal(different_values_model));

        const_model_type different_values_const_model(other_vector2.data(), 3);
        REQUIRE_FALSE(const_holder.are_equal(different_values_const_model));

        // Different types
        using other_model_type       = ContiguousViewModel<other_t>;
        using const_other_model_type = ContiguousViewModel<const other_t>;
        std::vector<other_t> other_vector3{1.0, 2.0, 3.0};

        other_model_type other_model(other_vector3.data(), 3);
        REQUIRE_FALSE(holder.are_equal(other_model));

        const_other_model_type other_const_model(other_vector3.data(), 3);
        REQUIRE_FALSE(const_holder.are_equal(other_const_model));
    }
}