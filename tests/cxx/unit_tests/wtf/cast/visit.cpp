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
#include <wtf/cast/visit.hpp>
#include <wtf/fp/float.hpp>

using namespace test_wtf;

TEST_CASE("wtf::cast::visit", "[wtf]") {
    SECTION("Float") {
        auto f      = wtf::fp::make_float(3.14f);
        bool called = false;
        wtf::cast::visit<default_fp_types>(
          [&](auto v) {
              called = true;
              REQUIRE(v == 3.14f);
          },
          f);
        REQUIRE(called);
    }

    SECTION("FloatView") {
        auto f      = wtf::fp::make_float(3.14f);
        auto view   = f.as_view();
        bool called = false;
        wtf::cast::visit<default_fp_types>(
          [&](auto v) {
              called = true;
              REQUIRE(v == 3.14f);
          },
          view);
        REQUIRE(called);
    }

    SECTION("FloatBuffer") {
        wtf::buffer::FloatBuffer buffer({1.0f, 2.0f, 3.0f});
        bool called = false;
        wtf::cast::visit<default_fp_types>(
          [&](auto s) {
              called = true;
              REQUIRE(s.size() == 3);
          },
          buffer);
        REQUIRE(called);
    }

    SECTION("BufferView") {
        wtf::buffer::FloatBuffer buffer({1.0f, 2.0f, 3.0f});
        auto view   = buffer.as_view();
        bool called = false;
        wtf::cast::visit<default_fp_types>(
          [&](auto s) {
              called = true;
              REQUIRE(s.size() == 3);
          },
          view);
        REQUIRE(called);
    }
}
