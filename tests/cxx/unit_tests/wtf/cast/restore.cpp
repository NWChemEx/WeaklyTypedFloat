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
#include <wtf/cast/detail_/restore.hpp>
#include <wtf/fp/detail_/float_model.hpp>

using namespace wtf::cast::detail_;
using wtf::fp::detail_::FloatModel;

TEST_CASE("restore", "[wtf]") {
    FloatModel<float> model(3.14f);
    wtf::fp::detail_::FloatHolder& holder = model;

    SECTION("Downcasts to the matching model") {
        auto& r = restore<FloatModel, float>(holder, "bad cast");
        REQUIRE(&r == &model);
        REQUIRE(*r.data() == 3.14f);
    }

    SECTION("Throws on a mismatch") {
        REQUIRE_THROWS_AS((restore<FloatModel, double>(holder, "bad cast")),
                          std::runtime_error);
    }

    SECTION("Propagates the caller-supplied message") {
        try {
            restore<FloatModel, double>(holder, "custom message");
            FAIL("restore() should have thrown");
        } catch(const std::runtime_error& e) {
            REQUIRE(std::string(e.what()) == "custom message");
        }
    }
}
