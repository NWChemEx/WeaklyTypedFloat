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

#include <wtf/rtti/detail_/type_holder.hpp>
#include <wtf/rtti/detail_/type_model.hpp>

namespace wtf::rtti::detail_ {

using string_type    = TypeHolder::string_type;
using holder_pointer = TypeHolder::holder_pointer;

std::map<string_type, holder_pointer> TypeHolder::m_registry_;

} // namespace wtf::rtti::detail_
