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
#include <wtf/fp/float.hpp>
#include <wtf/fp/float_view.hpp>

using namespace wtf::fp;
using namespace test_wtf;

namespace {

/// A user-defined policy, declared with no changes to WTF, that halves the
/// held value on the way to T. Exercises the "open to user extension"
/// requirement.
struct HalvingPolicy {
    template<typename T, typename U>
    static std::optional<T> convert(U&& value) {
        return static_cast<T>(value) / static_cast<T>(2);
    }
};

} // namespace

TEST_CASE("convert_to(FloatView<const Float>)", "[wtf]") {
    auto f = make_float(3.14f);

    SECTION("Match: same type succeeds") {
        REQUIRE(convert_to<float>(f) == 3.14f);
    }

    SECTION("Match: different type throws") {
        REQUIRE_THROWS_AS((convert_to<double>(f)), std::runtime_error);
    }

    SECTION("Convertible: widening succeeds") {
        REQUIRE(convert_to<double, policies::Convertible>(f) ==
                Catch::Approx(3.14).epsilon(1e-6));
    }

    SECTION("Convertible: narrowing also succeeds") {
        auto d = make_float(3.14);
        REQUIRE(convert_to<float, policies::Convertible>(d) ==
                Catch::Approx(3.14f));
    }

    SECTION("Widen: widening succeeds") {
        REQUIRE(convert_to<double, policies::Widen>(f) ==
                Catch::Approx(3.14).epsilon(1e-6));
    }

    SECTION("Widen: narrowing throws") {
        auto d = make_float(3.14);
        REQUIRE_THROWS_AS((convert_to<float, policies::Widen>(d)),
                          std::runtime_error);
    }

    SECTION("A user-defined policy works with no WTF changes") {
        REQUIRE(convert_to<float, HalvingPolicy>(f) == 1.57f);
    }

    SECTION("A narrowed TupleType throws even when Match would otherwise "
            "succeed") {
        REQUIRE_THROWS_AS(
          (convert_to<float, policies::Match, std::tuple<double>>(f)),
          std::runtime_error);
    }

    SECTION("Works directly on a Float, via as_view()") {
        REQUIRE(convert_to<float>(f) == 3.14f);
    }

    SECTION("Works on an explicit FloatView") {
        auto view = f.as_view();
        REQUIRE(convert_to<float>(view) == 3.14f);
    }

    SECTION("Never aliases: the result is a copy, not a reference") {
        auto copy = convert_to<float>(f);
        copy      = 1.0f;
        REQUIRE(float_cast<float>(f) == 3.14f);
    }
}
