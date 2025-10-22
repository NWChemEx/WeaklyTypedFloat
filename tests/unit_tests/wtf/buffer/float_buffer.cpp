#include "../../../test_wtf.hpp"
#include <wtf/buffer/float_buffer.hpp>

using namespace wtf::buffer;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("FloatBuffer", "[wtf]", default_fp_types) {
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    vector_type empty_vector{};
    auto buffer = make_float_buffer(val.begin(), val.end());
    auto empty  = make_float_buffer(empty_vector.begin(), empty_vector.end());

    SECTION("ctors and assignment") {
        SECTION("By vector") {
            FloatBuffer buf_from_vector(val);
            REQUIRE(buf_from_vector.size() == 3);
            REQUIRE(buf_from_vector.at(0) == one);
            REQUIRE(buf_from_vector.at(1) == two);
            REQUIRE(buf_from_vector.at(2) == three);
        }

        SECTION("By iterators") {
            FloatBuffer buf_from_iterators(val.begin(), val.end());
            REQUIRE(buf_from_iterators.size() == 3);
            REQUIRE(buf_from_iterators.at(0) == one);
            REQUIRE(buf_from_iterators.at(1) == two);
            REQUIRE(buf_from_iterators.at(2) == three);
        }

        SECTION("copy ctor") {
            FloatBuffer copy_buffer(buffer);
            REQUIRE(copy_buffer == buffer);

            auto copy_elem0 = copy_buffer.at(0);
            auto orig_elem0 = buffer.at(0);
            auto& copy0     = float_cast<TestType&>(copy_elem0);
            auto& orig0     = float_cast<TestType&>(orig_elem0);
            REQUIRE(&copy0 != &orig0); // Ensure deep copy
        }

        SECTION("copy assignment") {
            auto pempty_buffer = &(empty = buffer);
            REQUIRE(empty == buffer);
            REQUIRE(pempty_buffer == &empty);

            auto copy_elem0 = empty.at(0);
            auto orig_elem0 = buffer.at(0);
            auto& copy0     = float_cast<TestType&>(copy_elem0);
            auto& orig0     = float_cast<TestType&>(orig_elem0);
            REQUIRE(&copy0 != &orig0); // Ensure deep copy
        }

        SECTION("move ctor") {
            FloatBuffer moved_buffer(std::move(buffer));
            REQUIRE(moved_buffer.size() == 3);
            REQUIRE(moved_buffer.at(0) == one);
            REQUIRE(moved_buffer.at(1) == two);
            REQUIRE(moved_buffer.at(2) == three);
        }

        SECTION("move assignment") {
            auto pempty_buffer = &(empty = std::move(buffer));
            REQUIRE(empty.size() == 3);
            REQUIRE(empty.at(0) == one);
            REQUIRE(empty.at(1) == two);
            REQUIRE(empty.at(2) == three);
            REQUIRE(pempty_buffer == &empty);
        }
    }

    SECTION("at()") {
        REQUIRE(buffer.at(0) == one);
        REQUIRE(buffer.at(1) == two);
        REQUIRE(buffer.at(2) == three);

        REQUIRE_THROWS_AS(buffer.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(empty.at(0), std::out_of_range);

        // Can write to it
        buffer.at(0) = TestType{4.0};
        REQUIRE(buffer.at(0) == TestType{4.0});
    }

    SECTION("at() const") {
        const auto& cholder       = buffer;
        const auto& cempty_holder = empty;
        REQUIRE(cholder.at(0) == one);
        REQUIRE(cholder.at(1) == two);
        REQUIRE(cholder.at(2) == three);

        REQUIRE_THROWS_AS(cholder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cempty_holder.at(0), std::out_of_range);
    }

    SECTION("size()") {
        REQUIRE(buffer.size() == 3);
        REQUIRE(empty.size() == 0);
    }

    SECTION("is_contiguous()") {
        REQUIRE(buffer.is_contiguous());
        REQUIRE(empty.is_contiguous());
    }

    SECTION("operator==") {
        // Same contents
        REQUIRE(buffer == make_float_buffer(val.begin(), val.end()));
        REQUIRE(empty ==
                make_float_buffer(empty_vector.begin(), empty_vector.end()));

        // Different sizes
        REQUIRE_FALSE(buffer == empty);

        // Different values
        vector_type different_val{one, two, TestType{4.0}};
        REQUIRE_FALSE(buffer == make_float_buffer(different_val.begin(),
                                                  different_val.end()));

        // Different types
        std::vector<other_t> other_vector{1.0, 2.0, 3.0};
        auto other_buffer =
          make_float_buffer(other_vector.begin(), other_vector.end());
        REQUIRE_FALSE(buffer == other_buffer);
    }

    SECTION("operator!=") {
        // Just negates operator==, so spot checking is fine
        REQUIRE_FALSE(buffer != make_float_buffer(val.begin(), val.end()));
        REQUIRE(buffer != empty);
    }

    SECTION("value()") {
        auto span = buffer.template value<TestType>();
        REQUIRE(span.size() == 3);
        REQUIRE(span[0] == one);
        REQUIRE(span[1] == two);
        REQUIRE(span[2] == three);

        auto empty_span = empty.template value<TestType>();
        REQUIRE(empty_span.size() == 0);

        // Modify through the span
        span[0] = TestType{5.0};
        REQUIRE(buffer.at(0) == TestType{5.0});

        // Wrong type
        REQUIRE_THROWS_AS(buffer.template value<other_t>(), std::runtime_error);
    }

    SECTION("value() const") {
        const auto& cbuffer = buffer;
        auto span           = cbuffer.template value<TestType>();
        REQUIRE(span.size() == 3);
        REQUIRE(span[0] == one);
        REQUIRE(span[1] == two);
        REQUIRE(span[2] == three);

        const auto& cempty = empty;
        auto empty_span    = cempty.template value<TestType>();
        REQUIRE(empty_span.size() == 0);

        // Wrong type
        REQUIRE_THROWS_AS(cbuffer.template value<other_t>(),
                          std::runtime_error);
    }
}

TEMPLATE_LIST_TEST_CASE("make_float_buffer", "[wtf]", all_fp_types) {
    using vector_type = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};

    auto buffer = make_float_buffer(val);
    REQUIRE(buffer.size() == 3);
    REQUIRE(buffer.at(0) == one);
    REQUIRE(buffer.at(1) == two);
    REQUIRE(buffer.at(2) == three);
}

TEMPLATE_LIST_TEST_CASE("contiguous_buffer_cast", "[wtf]", all_fp_types) {
    using vector_type = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    auto buffer = make_float_buffer(val);
    auto span   = contiguous_buffer_cast<TestType>(buffer);
    REQUIRE(span.size() == 3);
    REQUIRE(span[0] == one);
    REQUIRE(span[1] == two);
    REQUIRE(span[2] == three);
}