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
#include <unordered_set>
#include <wtf/enums/enums.hpp>

using namespace wtf::enums;

TEST_CASE("FloatKind", "[wtf][enums]") {
    SECTION("Default types are pre-registered") {
        REQUIRE_NOTHROW(from_string("float"));
        REQUIRE_NOTHROW(from_string("double"));
        REQUIRE_NOTHROW(from_string("long double"));
    }

    SECTION("to_string / from_string round trip") {
        auto k = float_kind<double>();
        REQUIRE(to_string(k) == "double");
        REQUIRE(from_string("double") == k);
    }

    SECTION("Unknown name throws") {
        REQUIRE_THROWS_AS(from_string("not_a_registered_type"),
                          std::runtime_error);
    }

    SECTION("Macro-registered custom type is look-up-able") {
        // test_wtf::MyCustomFloat is registered via WTF_REGISTER_FP_TYPE in
        // test_wtf.hpp, exercising the macro's auto-registration.
        auto k = float_kind<test_wtf::MyCustomFloat>();
        REQUIRE(to_string(k) == "test_wtf::MyCustomFloat");
        REQUIRE(from_string("test_wtf::MyCustomFloat") == k);
    }

    SECTION("Equality/ordering") {
        auto kf = float_kind<float>();
        auto kd = float_kind<double>();
        REQUIRE(kf == float_kind<float>());
        REQUIRE(kf != kd);
        REQUIRE((kf < kd || kd < kf));
        REQUIRE_FALSE(kf < kf);
    }

    SECTION("float_kind<T> is idempotent") {
        REQUIRE(float_kind<float>() == float_kind<float>());
    }

    SECTION("Usable as a key in an unordered container") {
        std::unordered_set<FloatKind> kinds{float_kind<float>(),
                                            float_kind<double>()};
        REQUIRE(kinds.count(float_kind<float>()) == 1);
        REQUIRE(kinds.count(float_kind<long double>()) == 0);
    }
}

TEST_CASE("dispatch_by_kind", "[wtf][enums]") {
    using types = std::tuple<float, double>;

    SECTION("Dispatches to the matching type") {
        auto result = detail_::dispatch_by_kind<types>(
          float_kind<double>(),
          [&]<typename T>() { return std::is_same_v<T, double>; });
        REQUIRE(result);
    }

    SECTION("Throws if kind is not in the candidate tuple") {
        REQUIRE_THROWS_AS(
          detail_::dispatch_by_kind<types>(
            float_kind<long double>(),
            [&]<typename T>() { return std::is_same_v<T, double>; }),
          std::runtime_error);
    }
}
