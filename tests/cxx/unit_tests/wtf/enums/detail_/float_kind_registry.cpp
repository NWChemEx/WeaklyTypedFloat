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

#include "../../../../test_wtf.hpp"
#include <wtf/enums/detail_/float_kind_registry.hpp>

using namespace wtf::enums::detail_;

TEST_CASE("FloatKindRegistry", "[wtf][enums]") {
    auto& reg = FloatKindRegistry::instance();

    SECTION("Seeded with the default FP types") {
        REQUIRE(reg.size() >= 3);
        REQUIRE_NOTHROW(reg.id_of("float"));
        REQUIRE_NOTHROW(reg.id_of("double"));
        REQUIRE_NOTHROW(reg.id_of("long double"));
    }

    SECTION("register_name is idempotent") {
        auto n   = reg.size();
        auto id1 = reg.register_name("FloatKindRegistryTest::SomeType");
        REQUIRE(reg.size() == n + 1);

        auto id2 = reg.register_name("FloatKindRegistryTest::SomeType");
        REQUIRE(id1 == id2);
        REQUIRE(reg.size() == n + 1);
    }

    SECTION("id_of throws for an unregistered name") {
        REQUIRE_THROWS_AS(reg.id_of("FloatKindRegistryTest::NotRegistered"),
                          std::runtime_error);
    }

    SECTION("name_of round-trips with register_name") {
        auto id = reg.register_name("FloatKindRegistryTest::AnotherType");
        REQUIRE(reg.name_of(id) == "FloatKindRegistryTest::AnotherType");
    }

    SECTION("name_of throws for an unregistered id") {
        REQUIRE_THROWS_AS(reg.name_of(reg.size() + 100), std::out_of_range);
    }

    SECTION("instance() always returns the same registry") {
        REQUIRE(&FloatKindRegistry::instance() == &reg);
    }
}
