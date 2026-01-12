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

#include <wtf/rtti/type_info.hpp>

namespace wtf::rtti {
using holder_reference       = TypeInfo::holder_reference;
using const_holder_reference = TypeInfo::const_holder_reference;
using const_string_reference = TypeInfo::const_string_reference;

TypeInfo::TypeInfo(holder_pointer holder) : m_holder_(std::move(holder)) {}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

const_string_reference TypeInfo::name() const noexcept {
    return holder_().name();
}

// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

bool TypeInfo::operator==(const TypeInfo& other) const {
    return holder_().are_equal(other.holder_());
}

bool TypeInfo::operator!=(const TypeInfo& other) const {
    return !(*this == other);
}

// -----------------------------------------------------------------------------
// Private methods
// -----------------------------------------------------------------------------

holder_reference TypeInfo::holder_() { return *m_holder_; }

const_holder_reference TypeInfo::holder_() const { return *m_holder_; }

} // namespace wtf::rtti
