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
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/buffer/float_buffer.hpp>

using namespace wtf::buffer;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("BufferView", "[wtf]", default_fp_types) {
    using view_type         = BufferView<wtf::fp::Float>;
    using const_view_type   = BufferView<const wtf::fp::Float>;
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    vector_type empty_vector{};

    view_type defaulted;
    view_type buffer(val.data(), 3);
    view_type empty(empty_vector.data(), 0);

    const_view_type const_defaulted;
    const_view_type const_buffer(val.data(), 3);
    const_view_type const_empty(empty_vector.data(), 0);

    SECTION("ctors and assignment") {
        SECTION("default ctor") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(const_defaulted.size() == 0);
        }

        SECTION("From FloatBuffer") {
            FloatBuffer fb(val);
            view_type from_fb(fb);
            REQUIRE(from_fb.size() == 3);
            REQUIRE(from_fb.at(0) == one);
            REQUIRE(from_fb.at(1) == two);
            REQUIRE(from_fb.at(2) == three);
        }

        SECTION("By pointer") {
            REQUIRE(buffer.size() == 3);
            REQUIRE(buffer.at(0) == one);
            REQUIRE(buffer.at(1) == two);
            REQUIRE(buffer.at(2) == three);

            auto& buf_elem0 = float_cast<TestType&>(buffer.at(0));
            REQUIRE(&buf_elem0 == &val[0]); // Ensure aliasing

            REQUIRE(empty.size() == 0);

            REQUIRE(const_buffer.size() == 3);
            REQUIRE(const_buffer.at(0) == one);
            REQUIRE(const_buffer.at(1) == two);
            REQUIRE(const_buffer.at(2) == three);
            auto& celem0 = float_cast<const TestType&>(const_buffer.at(0));
            REQUIRE(&celem0 == &val[0]); // Ensure aliasing

            REQUIRE(const_empty.size() == 0);
        }

        SECTION("const from non-const") {
            const_view_type const_from_buffer(buffer);
            REQUIRE(const_from_buffer == const_buffer);
            auto& celem0 = float_cast<const TestType&>(const_from_buffer.at(0));
            auto& orig_elem0 = float_cast<TestType&>(buffer.at(0));
            REQUIRE(&celem0 == &orig_elem0); // Ensure shallow copy

            const_view_type const_from_empty(empty);
            REQUIRE(const_from_empty == const_empty);
        }

        SECTION("copy ctor") {
            view_type copy_defaulted(defaulted);
            REQUIRE(copy_defaulted.size() == 0);

            view_type copy_buffer(buffer);
            REQUIRE(copy_buffer == buffer);

            auto copy_elem0 = copy_buffer.at(0);
            auto orig_elem0 = buffer.at(0);
            auto& copy0     = float_cast<TestType&>(copy_elem0);
            auto& orig0     = float_cast<TestType&>(orig_elem0);
            REQUIRE(&copy0 == &orig0); // Ensure shallow copy

            const_view_type copy_const_buffer(const_buffer);
            REQUIRE(copy_const_buffer == const_buffer);

            auto copy_const_elem0 = copy_const_buffer.at(0);
            auto orig_const_elem0 = const_buffer.at(0);
            auto& copyc0 = float_cast<const TestType&>(copy_const_elem0);
            auto& origc0 = float_cast<const TestType&>(orig_const_elem0);
            REQUIRE(&copyc0 == &origc0); // Ensure shallow copy
        }

        SECTION("copy assignment") {
            SECTION("Copy from defaulted") {
                auto pbuffer = &(buffer = defaulted);
                REQUIRE(buffer.size() == 0);
                REQUIRE(pbuffer == &buffer);
            }

            SECTION("Copy to defaulted") {
                auto pdefaulted = &(defaulted = buffer);
                REQUIRE(defaulted == buffer);
                REQUIRE(pdefaulted == &defaulted);
            }

            SECTION("copy from non-empty") {
                auto pempty_buffer = &(empty = buffer);
                REQUIRE(empty == buffer);
                REQUIRE(pempty_buffer == &empty);

                auto copy_elem0 = empty.at(0);
                auto orig_elem0 = buffer.at(0);
                auto& copy0     = float_cast<TestType&>(copy_elem0);
                auto& orig0     = float_cast<TestType&>(orig_elem0);
                REQUIRE(&copy0 == &orig0); // Ensure shallow copy
            }

            SECTION("copy from const non-empty") {
                auto pconst_empty_buffer = &(const_empty = const_buffer);
                REQUIRE(const_empty == const_buffer);
                REQUIRE(pconst_empty_buffer == &const_empty);

                auto copy_const_elem0 = const_empty.at(0);
                auto orig_const_elem0 = const_buffer.at(0);
                auto& copyc0 = float_cast<const TestType&>(copy_const_elem0);
                auto& origc0 = float_cast<const TestType&>(orig_const_elem0);
                REQUIRE(&copyc0 == &origc0); // Ensure shallow copy
            }
        }

        SECTION("move ctor") {
            view_type moved_buffer(std::move(buffer));
            REQUIRE(moved_buffer.size() == 3);
            REQUIRE(moved_buffer.at(0) == one);
            REQUIRE(moved_buffer.at(1) == two);
            REQUIRE(moved_buffer.at(2) == three);
            auto& mb_elem0 = float_cast<TestType&>(moved_buffer.at(0));
            REQUIRE(&mb_elem0 == &val[0]); // Ensure aliasing

            const_view_type moved_const_buffer(std::move(const_buffer));
            REQUIRE(moved_const_buffer.size() == 3);
            REQUIRE(moved_const_buffer.at(0) == one);
            REQUIRE(moved_const_buffer.at(1) == two);
            REQUIRE(moved_const_buffer.at(2) == three);
            auto& mcb_elem0 =
              float_cast<const TestType&>(moved_const_buffer.at(0));
            REQUIRE(&mcb_elem0 == &val[0]); // Ensure aliasing
        }

        SECTION("move assignment") {
            auto pempty_buffer = &(empty = std::move(buffer));
            REQUIRE(empty.size() == 3);
            REQUIRE(empty.at(0) == one);
            REQUIRE(empty.at(1) == two);
            REQUIRE(empty.at(2) == three);
            REQUIRE(pempty_buffer == &empty);
            auto& eb_elem0 = float_cast<TestType&>(empty.at(0));
            REQUIRE(&eb_elem0 == &val[0]); // Ensure aliasing

            auto pconst_empty_buffer = &(const_empty = std::move(const_buffer));
            REQUIRE(const_empty.size() == 3);
            REQUIRE(const_empty.at(0) == one);
            REQUIRE(const_empty.at(1) == two);
            REQUIRE(const_empty.at(2) == three);
            REQUIRE(pconst_empty_buffer == &const_empty);
            auto& ceb_elem0 = float_cast<const TestType&>(const_empty.at(0));
            REQUIRE(&ceb_elem0 == &val[0]); // Ensure aliasing
        }
    }

    SECTION("at()") {
        REQUIRE(buffer.at(0) == one);
        REQUIRE(buffer.at(1) == two);
        REQUIRE(buffer.at(2) == three);
        auto& elem0 = float_cast<TestType&>(buffer.at(0));
        REQUIRE(&elem0 == val.data()); // Alias check

        REQUIRE(const_buffer.at(0) == one);
        REQUIRE(const_buffer.at(1) == two);
        REQUIRE(const_buffer.at(2) == three);
        const auto& celem0 = float_cast<const TestType&>(const_buffer.at(0));
        REQUIRE(&celem0 == val.data());

        REQUIRE_THROWS_AS(defaulted.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(buffer.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(empty.at(0), std::out_of_range);

        REQUIRE_THROWS_AS(const_defaulted.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(const_buffer.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(const_empty.at(0), std::out_of_range);

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
        const auto& celem0 = float_cast<const TestType&>(cholder.at(0));
        REQUIRE(&celem0 == val.data());

        const auto& cconst_defaulted    = const_defaulted;
        const auto& cconst_holder       = const_buffer;
        const auto& cconst_empty_holder = const_empty;
        REQUIRE(cconst_holder.at(0) == one);
        REQUIRE(cconst_holder.at(1) == two);
        REQUIRE(cconst_holder.at(2) == three);
        const auto& ccelem0 = float_cast<const TestType&>(cconst_holder.at(0));
        REQUIRE(&ccelem0 == val.data());

        REQUIRE_THROWS_AS(std::as_const(defaulted).at(0), std::out_of_range);
        REQUIRE_THROWS_AS(cholder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cempty_holder.at(0), std::out_of_range);

        REQUIRE_THROWS_AS(cconst_defaulted.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(cconst_holder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cconst_empty_holder.at(0), std::out_of_range);
    }

    SECTION("size()") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(buffer.size() == 3);
        REQUIRE(empty.size() == 0);

        REQUIRE(const_defaulted.size() == 0);
        REQUIRE(const_buffer.size() == 3);
        REQUIRE(const_empty.size() == 0);
    }

    SECTION("is_contiguous()") {
        REQUIRE(defaulted.is_contiguous());
        REQUIRE(buffer.is_contiguous());
        REQUIRE(empty.is_contiguous());

        REQUIRE(const_defaulted.is_contiguous());
        REQUIRE(const_buffer.is_contiguous());
        REQUIRE(const_empty.is_contiguous());
    }

    SECTION("operator==") {
        // Same contents
        REQUIRE(defaulted == view_type{});
        REQUIRE(buffer == view_type(val.data(), 3));
        REQUIRE(empty == view_type(empty_vector.data(), 0));

        REQUIRE(const_defaulted == const_view_type{});
        REQUIRE(const_buffer == const_view_type(val.data(), 3));
        REQUIRE(const_empty == const_view_type(empty_vector.data(), 0));

        // Empty equals defaulted
        REQUIRE(defaulted == empty);
        REQUIRE(const_defaulted == const_empty);

        // Different sizes
        REQUIRE_FALSE(defaulted == buffer);
        REQUIRE_FALSE(buffer == empty);
        REQUIRE_FALSE(const_defaulted == const_buffer);
        REQUIRE_FALSE(const_buffer == const_empty);

        // Different values
        vector_type different_val{one, two, TestType{4.0}};
        REQUIRE_FALSE(buffer == view_type(different_val.data(), 3));
        const_view_type different_const_view(different_val.data(), 3);
        REQUIRE_FALSE(const_buffer == different_const_view);

        // Different types
        std::vector<other_t> other_vector{1.0, 2.0, 3.0};
        auto other_buffer = view_type(other_vector.data(), 3);
        REQUIRE_FALSE(buffer == other_buffer);

        auto const_other_buffer = const_view_type(other_vector.data(), 3);
        REQUIRE_FALSE(const_buffer == const_other_buffer);

        // Different const-ness
        REQUIRE(defaulted == const_defaulted);
        REQUIRE(buffer == const_buffer);
        REQUIRE(empty == const_empty);
    }

    SECTION("operator!=") {
        // Just negates operator==, so spot checking is fine
        REQUIRE_FALSE(buffer != view_type(val.data(), 3));
        REQUIRE(buffer != empty);

        REQUIRE_FALSE(const_buffer != const_view_type(val.data(), 3));
        REQUIRE(const_buffer != const_empty);

        // Different const-ness
        REQUIRE_FALSE(buffer != const_buffer);
    }

    SECTION("value()") {
        REQUIRE(defaulted.template value<TestType>().size() == 0);
        REQUIRE(const_defaulted.template value<const TestType>().size() == 0);

        auto span = buffer.template value<TestType>();
        REQUIRE(span.size() == 3);
        REQUIRE(span[0] == one);
        REQUIRE(span[1] == two);
        REQUIRE(span[2] == three);

        auto const_span = const_buffer.template value<const TestType>();
        REQUIRE(const_span.size() == 3);
        REQUIRE(const_span[0] == one);
        REQUIRE(const_span[1] == two);
        REQUIRE(const_span[2] == three);

        auto empty_span = empty.template value<TestType>();
        REQUIRE(empty_span.size() == 0);

        auto const_empty_span = const_empty.template value<const TestType>();
        REQUIRE(const_empty_span.size() == 0);

        // Modify through the span
        span[0] = TestType{5.0};
        REQUIRE(buffer.at(0) == TestType{5.0});

        // Wrong type
        REQUIRE_THROWS_AS(buffer.template value<other_t>(), std::runtime_error);
        REQUIRE_THROWS_AS(const_buffer.template value<const other_t>(),
                          std::runtime_error);
    }

    SECTION("value() const") {
        const auto& cdefaulted       = defaulted;
        const auto& cconst_defaulted = const_defaulted;
        REQUIRE(cdefaulted.template value<TestType>().size() == 0);
        REQUIRE(cconst_defaulted.template value<const TestType>().size() == 0);

        const auto& cbuffer = buffer;
        auto span           = cbuffer.template value<TestType>();
        REQUIRE(span.size() == 3);
        REQUIRE(span[0] == one);
        REQUIRE(span[1] == two);
        REQUIRE(span[2] == three);

        const auto& cconst_buffer = const_buffer;
        auto const_span           = cconst_buffer.template value<TestType>();
        REQUIRE(const_span.size() == 3);
        REQUIRE(const_span[0] == one);
        REQUIRE(const_span[1] == two);
        REQUIRE(const_span[2] == three);

        const auto& cempty = empty;
        auto empty_span    = cempty.template value<TestType>();
        REQUIRE(empty_span.size() == 0);

        const auto& cconst_empty = const_empty;
        auto const_empty_span    = cconst_empty.template value<TestType>();
        REQUIRE(const_empty_span.size() == 0);

        // Wrong type
        REQUIRE_THROWS_AS(cbuffer.template value<other_t>(),
                          std::runtime_error);
        REQUIRE_THROWS_AS(cconst_buffer.template value<other_t>(),
                          std::runtime_error);
    }
}

TEMPLATE_LIST_TEST_CASE("make_buffer_view", "[wtf]", all_fp_types) {
    using vector_type = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};

    auto buffer = make_buffer_view(val.data(), 3);
    REQUIRE(buffer.size() == 3);
    REQUIRE(buffer.at(0) == one);
    REQUIRE(buffer.at(1) == two);
    REQUIRE(buffer.at(2) == three);

    const auto* cdata = val.data();
    auto const_buffer = make_buffer_view(cdata, 3);
    REQUIRE(const_buffer.size() == 3);
    REQUIRE(const_buffer.at(0) == one);
    REQUIRE(const_buffer.at(1) == two);
    REQUIRE(const_buffer.at(2) == three);
}

TEMPLATE_LIST_TEST_CASE("contiguous_buffer_cast(BufferView)", "[wtf]",
                        all_fp_types) {
    using vector_type = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    auto buffer = make_buffer_view(val.data(), 3);
    auto span   = contiguous_buffer_cast<TestType>(buffer);
    REQUIRE(span.size() == 3);
    REQUIRE(span[0] == one);
    REQUIRE(span[1] == two);
    REQUIRE(span[2] == three);
    REQUIRE(span.data() == val.data());

    const auto* cdata = val.data();
    auto const_buffer = make_buffer_view(cdata, 3);
    auto const_span   = contiguous_buffer_cast<const TestType>(const_buffer);
    REQUIRE(const_span.size() == 3);
    REQUIRE(const_span[0] == one);
    REQUIRE(const_span[1] == two);
    REQUIRE(const_span[2] == three);
    REQUIRE(const_span.data() == cdata);
}
