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

namespace wtf::type_traits {

/** @brief WTF's trait for determining if @p T is a floating point type.
 *
 *  @param T The type to check.
 *
 *  While the C++ standard library provides `std::is_floating_point`, users
 *  should not extend it. This trait serves as a wrapper around
 *  `std::is_floating_point` that users can safely specialize in order to
 *  register new floating point types with WTF.
 *
 *  The primary template contains a static boolean member `value` that is set
 *  to true if `std::is_floating_point_v<T>` is true, and false otherwise.
 */
template<typename T>
struct IsFloatingPoint {
    constexpr static bool value = std::is_floating_point_v<T>;
};

/// Convenience variable template for grabbing `IsFloatingPoint<T>::value`
template<typename T>
constexpr bool is_floating_point_v = IsFloatingPoint<T>::value;

} // namespace wtf::type_traits
