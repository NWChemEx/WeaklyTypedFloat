#include "../../../../test_wtf.hpp"
#include <wtf/buffer/detail_/contiguous_model.hpp>

using namespace wtf::detail_;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("ContiguousModel", "[wtf]", default_fp_types) {
    using model_type = ContiguousModel<TestType>;
    // constexpr bool is_float = std::is_same_v<TestType, float>;
    // using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type = typename model_type::vector_type;

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

    SECTION("clone()") { REQUIRE(model.clone()->are_equal(model)); }

    SECTION("is_contiguous()") { REQUIRE(model.is_contiguous()); }

    SECTION("operator==") {
        REQUIRE(model == model_type(vector_type{one, two, three}));

        // Different values
        REQUIRE_FALSE(model == model_type(vector_type{one, one, one}));

        // Different lengths
        REQUIRE_FALSE(model == model_type(vector_type{one, two}));

        // Different order
        REQUIRE_FALSE(model == model_type(vector_type{two, one, three}));
    }
}