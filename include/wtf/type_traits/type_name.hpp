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
#include <wtf/concepts/floating_point.hpp>

namespace wtf::type_traits {

/** @brief Primary template for converting @p T to a string.
 *
 *  @tparam T A floating-point type without any type modifiers.
 *
 *  This is the primary template for mapping a type to a string. The primary
 *  template is not defined (we opted not to use typeid().name() because it is
 *  system-dependent and not very user friendly). Specializations are provided
 *  for C++'s built-in floating point types. User-defined floating point types
 *  can be supported by specializing this template for the user-defined type.
 */
template<concepts::UnmodifiedFloatingPoint T>
struct TypeName;

/// Specializes TypeName for float
template<>
struct TypeName<float> {
    static constexpr auto value = "float";
};

/// Specializes TypeName for double
template<>
struct TypeName<double> {
    static constexpr auto value = "double";
};

/// Specializes TypeName for long double
template<>
struct TypeName<long double> {
    static constexpr auto value = "long double";
};

/** @brief A variable template for converting @p T to a string.
 *
 *  @tparam T A floating-point type without any type modifiers.
 *
 *  This variable template is simply a convenient shorthand for getting at the
 *  `value` member of the TypeName<T> struct.
 *
 */
template<concepts::UnmodifiedFloatingPoint T>
constexpr auto type_name_v = TypeName<T>::value;

} // namespace wtf::type_traits
