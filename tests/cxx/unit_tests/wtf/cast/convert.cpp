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
#include <wtf/buffer/float_buffer.hpp>
#include <wtf/cast/convert.hpp>
#include <wtf/fp/float.hpp>

using namespace test_wtf;

TEST_CASE("wtf::cast::convert", "[wtf]") {
    SECTION("Float") {
        auto f = wtf::fp::make_float(3.14f);
        REQUIRE(wtf::cast::convert<float>(f) == wtf::fp::convert_to<float>(f));
    }

    SECTION("FloatView") {
        auto f    = wtf::fp::make_float(3.14f);
        auto view = f.as_view();
        REQUIRE(wtf::cast::convert<float>(view) ==
                wtf::fp::convert_to<float>(view));
    }

    SECTION("FloatBuffer") {
        wtf::buffer::FloatBuffer buffer({1.0f, 2.0f, 3.0f});
        auto result =
          wtf::cast::convert<double, wtf::fp::policies::Widen>(buffer);
        REQUIRE(result.size() == 3);
        REQUIRE(result[2] == 3.0);
    }

    SECTION("BufferView") {
        wtf::buffer::FloatBuffer buffer({1.0f, 2.0f, 3.0f});
        auto view = buffer.as_view();
        auto result =
          wtf::cast::convert<double, wtf::fp::policies::Widen>(view);
        REQUIRE(result.size() == 3);
        REQUIRE(result[2] == 3.0);
    }
}
