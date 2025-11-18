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
#include <wtf/concepts/wtf_float.hpp>

using namespace wtf;

TEST_CASE("WTFFloat concept", "[wtf]") {
    STATIC_REQUIRE(wtf::concepts::WTFFloat<fp::Float>);
    STATIC_REQUIRE(wtf::concepts::WTFFloat<const fp::Float>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<float>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<double>);
    STATIC_REQUIRE_FALSE(wtf::concepts::WTFFloat<long double>);
}
