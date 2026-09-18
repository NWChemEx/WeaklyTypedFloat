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

#include "../../../test_wtf.hpp"
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/buffer/float_buffer.hpp>

using namespace wtf::buffer;
using namespace test_wtf;

TEST_CASE("convert_to(BufferView<const Float>)", "[wtf]") {
    FloatBuffer buffer({1.0f, 2.0f, 3.0f});

    SECTION("Converts every element, in order") {
        auto result = convert_to<double, wtf::fp::policies::Widen>(buffer);
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == 1.0);
        REQUIRE(result[1] == 2.0);
        REQUIRE(result[2] == 3.0);
    }

    SECTION("Empty buffer converts to an empty vector") {
        FloatBuffer empty;
        empty.reserve<float>(0);
        auto result = convert_to<double>(empty);
        REQUIRE(result.empty());
    }

    SECTION("A failing element throws and does not return a partial result") {
        REQUIRE_THROWS_AS((convert_to<float, wtf::fp::policies::Widen>(
                            FloatBuffer(std::vector<double>{1.0, 2.0}))),
                          std::runtime_error);
    }

    SECTION("Works directly on a FloatBuffer, via as_view()") {
        auto result = convert_to<double, wtf::fp::policies::Widen>(buffer);
        REQUIRE(result.size() == 3);
    }

    SECTION("Works on an explicit BufferView") {
        auto view   = buffer.as_view();
        auto result = convert_to<double, wtf::fp::policies::Widen>(view);
        REQUIRE(result.size() == 3);
    }
}
