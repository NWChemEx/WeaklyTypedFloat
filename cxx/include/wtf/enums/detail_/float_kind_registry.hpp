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

#pragma once
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace wtf::enums::detail_ {

/** @brief Runtime registry mapping floating-point type names to ids.
 *
 *  This class implements a simple, name-keyed registry of ids. It is the
 *  backing store for wtf::enums::FloatKind: registering a name reserves the
 *  next available id for it (or returns the existing id if the name is
 *  already registered), and ids can be converted back to the name they were
 *  registered with. *this is a Meyer's singleton (see instance()) so that
 *  the same registry is shared across every translation unit.
 */
class FloatKindRegistry {
public:
    /// Type used to identify a registered name
    using id_type = std::size_t;

    /** @brief Returns the single instance of *this.
     *
     *  @return A reference to the registry shared by all callers.
     *
     *  @throw None No throw guarantee.
     */
    static FloatKindRegistry& instance();

    /** @brief Registers @p name with *this, if it is not already registered.
     *
     *  @param[in] name The name to register.
     *
     *  @return The id assigned to @p name. If @p name was already
     *          registered, this is the id it was originally assigned.
     *
     *  @throw std::bad_alloc if there is a problem allocating storage for the
     *                        new name. Strong throw guarantee.
     */
    id_type register_name(const std::string& name);

    /** @brief Returns the id assigned to @p name.
     *
     *  @param[in] name The name to look up.
     *
     *  @return The id assigned to @p name.
     *
     *  @throw std::runtime_error if @p name is not registered. Strong throw
     *                            guarantee.
     */
    id_type id_of(const std::string& name) const;

    /** @brief Returns the name registered to @p id.
     *
     *  @param[in] id The id to look up.
     *
     *  @return The name registered to @p id.
     *
     *  @throw std::out_of_range if @p id is not registered. Strong throw
     *                           guarantee.
     */
    const std::string& name_of(id_type id) const;

    /** @brief The number of names currently registered with *this.
     *
     *  @return The number of names currently registered with *this.
     *
     *  @throw None No throw guarantee.
     */
    std::size_t size() const noexcept;

private:
    /// Seeds *this with the names of wtf::default_fp_types
    FloatKindRegistry();

    /// Maps an id to the name it was registered with
    std::vector<std::string> m_names_;

    /// Maps a name to the id it was registered with
    std::unordered_map<std::string, id_type> m_ids_;
};

} // namespace wtf::enums::detail_
