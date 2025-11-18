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

#pragma once
#include <type_traits>
#include <wtf/forward.hpp>

namespace wtf::concepts {

/** @brief Determines if @p T is a wtf::buffer::FloatBuffer object.
 *
 *  @tparam T The type to check.
 */
template<typename T>
concept FloatBuffer = std::is_same_v<T, buffer::FloatBuffer> ||
                      std::is_same_v<T, const buffer::FloatBuffer>;

} // namespace wtf::concepts
