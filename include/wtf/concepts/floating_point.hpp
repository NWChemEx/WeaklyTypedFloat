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
#include <wtf/concepts/modifiers.hpp>
#include <wtf/type_traits/is_floating_point.hpp>

namespace wtf::concepts {

/** @brief Is @p T an unmodified floating point type?
 *
 *  @tparam T The type being checked.
 *
 *  This concept establishes that @p T is a floating point type and that it is
 *  not const- or volatile-qualified, nor is it a reference or pointer type,
 *  i.e., it is unmodified.
 */
template<typename T>
concept UnmodifiedFloatingPoint =
  Unmodified<T> && wtf::type_traits::is_floating_point_v<T>;

/** @brief Is @p T a possibly cv-qualified floating point type?
 *
 *  @tparam T The type being checked.
 *
 *  This concept establishes that @p T is a floating point type, allowing it to
 *  be cv-qualified, but not a reference or pointer type.
 */
template<typename T>
concept FloatingPoint =
  UnmodifiedFloatingPoint<std::remove_volatile_t<std::remove_const_t<T>>>;

/** @brief Is @p T a const-qualified floating point type?
 *
 *  @tparam T The type being checked.
 *
 *  This concept establishes that @p T is a floating point type and that it is
 *  const-qualified.
 */
template<typename T>
concept ConstFloatingPoint = FloatingPoint<T> && ConstQualified<T>;

} // namespace wtf::concepts
