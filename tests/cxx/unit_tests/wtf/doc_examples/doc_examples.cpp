/*
 * Copyright 2026 NWChemEx-Project
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

// The tagged regions in this file (marked with `// [tag]` / `// [/tag]`) are
// literalinclude'd verbatim into the Sphinx docs (docs/source/quickstart.rst,
// writing_apis.rst, restoring_the_type.rst). Keep the tags, and the prose
// that references them, in sync if you change this file.

#include "../../../test_wtf.hpp"
#include <wtf/buffer/float_buffer.hpp>
#include <wtf/cast/cast.hpp>
#include <wtf/cast/convert.hpp>
#include <wtf/cast/visit.hpp>
#include <wtf/fp/float.hpp>

using namespace test_wtf;

TEST_CASE("doc_examples: quickstart Float", "[doc_examples]") {
    // [quickstart-float]
    auto f = wtf::fp::make_float(3.14f); // type-erased, holds a float

    REQUIRE(f.to_string().find("3.14") != std::string::npos);
    REQUIRE(f == wtf::fp::make_float(3.14f)); // same type and value

    float value = wtf::cast::cast<float>(f); // un-erase back to float
    REQUIRE(value == 3.14f);
    // [/quickstart-float]
}

TEST_CASE("doc_examples: quickstart FloatBuffer", "[doc_examples]") {
    // [quickstart-buffer]
    wtf::buffer::FloatBuffer buffer({1.0f, 2.0f, 3.0f});

    REQUIRE(buffer.size() == 3);

    auto span = wtf::cast::cast<float>(buffer); // un-erase to std::span<float>
    REQUIRE(span[2] == 3.0f);
    // [/quickstart-buffer]
}

TEST_CASE("doc_examples: restoring the type, cast", "[doc_examples]") {
    // [restoring-cast]
    auto f = wtf::fp::make_float(3.14f); // holds a float

    float value = wtf::cast::cast<float>(f); // exact match
    REQUIRE(value == 3.14f);

    // wtf::cast::cast<double>(f) would throw std::runtime_error: f holds a
    // float, not a double.

    // Can get the value by reference
    float& pvalue = wtf::cast::cast<float&>(f);
    // [/restoring-cast]
}

TEST_CASE("doc_examples: restoring the type, visit", "[doc_examples]") {
    // [restoring-visit]
    auto f = wtf::fp::make_float(3.14f);

    wtf::cast::visit<default_fp_types>([](auto v) { REQUIRE(v == 3.14f); },
                                       f); // dispatches to the held type
    // [/restoring-visit]
}

TEST_CASE("doc_examples: restoring the type, convert", "[doc_examples]") {
    // [restoring-convert]
    auto f = wtf::fp::make_float(3.14f); // holds a float

    // Match: only succeeds when the held and requested types are identical.
    auto a = wtf::cast::convert<float, wtf::fp::policies::Match>(f);
    REQUIRE(a == 3.14f);

    // Convertible: succeeds whenever the held type converts to the
    // requested one, even if precision could be lost.
    auto b = wtf::cast::convert<double, wtf::fp::policies::Convertible>(f);
    REQUIRE(b == Catch::Approx(3.14));

    // Widen: like Convertible, but only for conversions that cannot lose
    // precision (float -> double is fine; double -> float would throw).
    auto c = wtf::cast::convert<double, wtf::fp::policies::Widen>(f);
    REQUIRE(c == Catch::Approx(3.14));
    // [/restoring-convert]
}
