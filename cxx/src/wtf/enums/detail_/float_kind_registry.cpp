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

#include <stdexcept>
#include <tuple>
#include <wtf/enums/detail_/float_kind_registry.hpp>
#include <wtf/type_traits/type_name.hpp>
#include <wtf/types.hpp>

namespace wtf::enums::detail_ {

FloatKindRegistry& FloatKindRegistry::instance() {
    static FloatKindRegistry registry;
    return registry;
}

FloatKindRegistry::id_type FloatKindRegistry::register_name(
  const std::string& name) {
    if(auto itr = m_ids_.find(name); itr != m_ids_.end()) return itr->second;
    auto id      = m_names_.size();
    m_ids_[name] = id;
    m_names_.push_back(name);
    return id;
}

FloatKindRegistry::id_type FloatKindRegistry::id_of(
  const std::string& name) const {
    if(auto itr = m_ids_.find(name); itr != m_ids_.end()) return itr->second;
    throw std::runtime_error("No FloatKind registered for name " + name);
}

const std::string& FloatKindRegistry::name_of(id_type id) const {
    return m_names_.at(id);
}

std::size_t FloatKindRegistry::size() const noexcept { return m_names_.size(); }

FloatKindRegistry::FloatKindRegistry() {
    std::apply(
      [this](auto... types) {
          (register_name(type_traits::type_name_v<decltype(types)>), ...);
      },
      default_fp_types{});
}

} // namespace wtf::enums::detail_
