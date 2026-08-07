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

namespace wtf::concepts {

/** @brief Determines if the type @p T can be inserted into an ostream.
 *
 *  @tparam T Type to check for stream insertion support.
 *
 *  The StreamInsertable concept is satisfied if an object of type @p T can be
 *  inserted into an std::ostream using the << operator. Falling usual C++
 *  conventions, this usually means that an object of type @p T can be
 *  printed.
 */
template<typename T>
concept StreamInsertable = requires(std::ostream& os, const T& obj) {
    { os << obj } -> std::same_as<std::ostream&>;
};

} // namespace wtf::concepts
