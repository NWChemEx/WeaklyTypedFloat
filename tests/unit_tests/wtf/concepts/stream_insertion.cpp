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
#include <wtf/concepts/stream_insertion.hpp>

namespace {

struct Insertable {};

[[maybe_unused]] std::ostream& operator<<(std::ostream& os, const Insertable&) {
    return os;
}

struct NotInsertable {};

} // namespace

TEST_CASE("StreamInsertable concept", "[wtf][concepts]") {
    using namespace wtf::concepts;

    STATIC_REQUIRE(StreamInsertable<Insertable>);
    STATIC_REQUIRE_FALSE(StreamInsertable<NotInsertable>);
    STATIC_REQUIRE(StreamInsertable<float>);
    STATIC_REQUIRE(StreamInsertable<double>);
}
