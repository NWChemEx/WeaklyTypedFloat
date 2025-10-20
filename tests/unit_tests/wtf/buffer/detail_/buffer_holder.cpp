#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_model.hpp>

using namespace wtf::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("BufferHolder", "[wtf]", default_fp_types) {
    using model_type        = ContiguousModel<TestType>;
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = typename model_type::vector_type;

    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    model_type model(std::move(val));
    auto& holder = model;

    SECTION("clone()") { REQUIRE(holder.clone()->are_equal(model)); }

    SECTION("are_equal()") {
        // Different types
        using other_model_type = ContiguousModel<other_t>;
        std::vector<other_t> other_vector{1.0, 2.0, 3.0};
        REQUIRE_FALSE(holder.are_equal(other_model_type(other_vector)));
    }
}