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
#include <wtf/cast/detail_/visit_as.hpp>
#include <wtf/fp/detail_/float_model.hpp>

using namespace wtf::cast::detail_;
using wtf::fp::detail_::FloatModel;

TEST_CASE("visit_as", "[wtf]") {
    FloatModel<float> model(3.14f);
    wtf::fp::detail_::FloatHolder& holder = model;

    SECTION("Calls the visitor with the restored model's handle()") {
        // std::visit (which dispatch/visit_as builds on) requires every
        // instantiation of the visitor to return the same type, so the
        // visitor must not return the (per-candidate-type-varying) handle
        // itself.
        bool called = false;
        visit_as<FloatModel, test_wtf::default_fp_types>(
          [&](auto&& value) {
              called = true;
              REQUIRE(value == 3.14f);
          },
          holder);
        REQUIRE(called);
    }

    SECTION("Throws if none of the candidates match") {
        REQUIRE_THROWS_AS(
          (visit_as<FloatModel, std::tuple<double, long double>>([](auto&&) {},
                                                                 holder)),
          std::runtime_error);
    }
}
