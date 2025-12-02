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
#include <wtf/buffer/float_buffer.hpp>
#include <wtf/fp/float.hpp>

using namespace wtf::buffer;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("FloatBuffer", "[wtf]", default_fp_types) {
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    using vector_type       = std::vector<TestType>;
    TestType one{1.0}, two{2.0}, three{3.0};
    vector_type val{one, two, three};
    vector_type empty_vector{};

    FloatBuffer defaulted;
    auto buffer = make_float_buffer(val.begin(), val.end());
    auto empty  = make_float_buffer(empty_vector.begin(), empty_vector.end());

    SECTION("ctors and assignment") {
        SECTION("default ctor") { REQUIRE(defaulted.size() == 0); }

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
            FloatBuffer copy_defaulted(defaulted);
            REQUIRE(copy_defaulted.size() == 0);

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

    SECTION("as_view()") {
        auto defaulted_view = defaulted.as_view();
        REQUIRE(defaulted_view.size() == 0);

        auto view = buffer.as_view();
        REQUIRE(view.size() == 3);
        REQUIRE(view.at(0) == one);
        REQUIRE(view.at(1) == two);
        REQUIRE(view.at(2) == three);
    }

    SECTION("as_view() const") {
        auto defaulted_view = std::as_const(defaulted).as_view();
        REQUIRE(defaulted_view.size() == 0);

        auto view = std::as_const(buffer).as_view();
        REQUIRE(view.size() == 3);
        REQUIRE(view.at(0) == one);
        REQUIRE(view.at(1) == two);
        REQUIRE(view.at(2) == three);
    }

    SECTION("at()") {
        REQUIRE(buffer.at(0) == one);
        REQUIRE(buffer.at(1) == two);
        REQUIRE(buffer.at(2) == three);

        REQUIRE_THROWS_AS(defaulted.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(buffer.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(empty.at(0), std::out_of_range);

        // Can write to it via a TestType object
        buffer.at(0) = TestType{4.0};
        REQUIRE(buffer.at(0) == TestType{4.0});
    }

    SECTION("at() const") {
        const auto& cholder       = buffer;
        const auto& cempty_holder = empty;
        REQUIRE(cholder.at(0) == one);
        REQUIRE(cholder.at(1) == two);
        REQUIRE(cholder.at(2) == three);

        REQUIRE_THROWS_AS(defaulted.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(cholder.at(3), std::out_of_range);
        REQUIRE_THROWS_AS(cempty_holder.at(0), std::out_of_range);
    }

    SECTION("size()") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(buffer.size() == 3);
        REQUIRE(empty.size() == 0);
    }

    SECTION("is_contiguous()") {
        REQUIRE(defaulted.is_contiguous());
        REQUIRE(buffer.is_contiguous());
        REQUIRE(empty.is_contiguous());
    }

    SECTION("operator==") {
        // Same contents
        REQUIRE(defaulted == FloatBuffer{});
        REQUIRE(buffer == make_float_buffer(val.begin(), val.end()));
        REQUIRE(empty ==
                make_float_buffer(empty_vector.begin(), empty_vector.end()));

        // Coded to empty is equal to defaulted
        REQUIRE(defaulted == empty);

        // Different sizes
        REQUIRE_FALSE(defaulted == buffer);
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
        REQUIRE(defaulted.template value<TestType>().size() == 0);

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
        const auto& cdefaulted = defaulted;
        REQUIRE(cdefaulted.template value<TestType>().size() == 0);

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

struct ConstVisitorConstSpan {
    ConstVisitorConstSpan(float* pdataf, double* pdatad) :
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

struct VisitorConstSpan {
    VisitorConstSpan(float* pdataf, double* pdatad) :
      pdataf_corr(pdataf), pdatad_corr(pdatad) {}

    template<typename T>
    auto operator()(std::span<T> span) {
        if constexpr(std::is_same_v<std::remove_const_t<T>, float>) {
            REQUIRE(span.data() == pdataf_corr);
            m_called_float = true;
        } else {
            REQUIRE(span.data() == pdatad_corr);
            m_called_double = true;
        }
    }

    template<typename T>
    auto operator()(std::span<const T> span) {
        if constexpr(std::is_same_v<std::remove_const_t<T>, float>) {
            REQUIRE(span.data() == pdataf_corr);
            m_called_cfloat = true;
        } else {
            REQUIRE(span.data() == pdatad_corr);
            m_called_cdouble = true;
        }
        REQUIRE(span.size() == 3);
    }

    auto operator()(std::span<float> lhs, std::span<double> rhs) {
        REQUIRE(lhs.data() == pdataf_corr);
        REQUIRE(lhs.size() == 3);
        REQUIRE(rhs.data() == pdatad_corr);
        REQUIRE(rhs.size() == 3);
        m_called_float_double = true;
    }

    template<typename T, typename U>
    auto operator()(std::span<T> lhs, std::span<U> rhs) const {
        throw std::runtime_error("Only float, double supported");
    }

    bool m_called_float        = false;
    bool m_called_cfloat       = false;
    bool m_called_double       = false;
    bool m_called_cdouble      = false;
    bool m_called_float_double = false;

    float* pdataf_corr;
    double* pdatad_corr;
};

TEST_CASE("visit_contiguous_buffer") {
    std::vector<float> valf{1.0, 2.0, 3.0};
    std::vector<double> vald{1.0, 2.0, 3.0};
    auto pdataf = valf.data();
    auto pdatad = vald.data();

    ConstVisitorConstSpan visitor(pdataf, pdatad);
    VisitorConstSpan visitor2(pdataf, pdatad);

    FloatBuffer modelf(std::move(valf));
    FloatBuffer modeld(std::move(vald));

    using type_tuple = std::tuple<float, double>;

    SECTION("one argument") {
        SECTION("call span<const float>/ span<const double> overloads") {
            visit_contiguous_buffer<type_tuple>(visitor, modelf);
            visit_contiguous_buffer<type_tuple>(visitor, std::as_const(modelf));
            visit_contiguous_buffer<type_tuple>(visitor, modeld);
            visit_contiguous_buffer<type_tuple>(visitor, std::as_const(modeld));
        }

        SECTION("call span<const T> overload") {
            const auto& cmodelf = modelf;
            visit_contiguous_buffer<type_tuple>(visitor2, cmodelf);
            REQUIRE(visitor2.m_called_cfloat);
            REQUIRE_FALSE(visitor2.m_called_float);

            const auto& cmodeld = modeld;
            visit_contiguous_buffer<type_tuple>(visitor2, cmodeld);
            REQUIRE(visitor2.m_called_cdouble);
            REQUIRE_FALSE(visitor2.m_called_double);
        }

        SECTION("calls span<T> overload") {
            visit_contiguous_buffer<type_tuple>(visitor2, modelf);
            REQUIRE_FALSE(visitor2.m_called_cfloat);
            REQUIRE(visitor2.m_called_float);

            visit_contiguous_buffer<type_tuple>(visitor2, modeld);
            REQUIRE_FALSE(visitor2.m_called_cdouble);
            REQUIRE(visitor2.m_called_double);
        }
    }

    SECTION("Two arguments") {
        visit_contiguous_buffer<type_tuple>(visitor, modelf, modeld);

        visit_contiguous_buffer<type_tuple>(visitor2, modelf, modeld);
        REQUIRE(visitor2.m_called_float_double);
    }
}
