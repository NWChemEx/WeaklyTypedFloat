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

#include <wtf/fp/detail_/float_holder.hpp>

namespace wtf::fp::detail_ {

void FloatHolder::change_value(const FloatHolder& other) {
    if(type() == other.type()) {
        change_value_(other);
    } else {
        throw std::invalid_argument(
          "FloatHolder::set_value: Types do not match");
    }
}

} // namespace wtf::fp::detail_
