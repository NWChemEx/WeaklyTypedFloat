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
#include <limits>
#include <wtf/concepts/floating_point.hpp>

namespace wtf::type_traits {

/** @brief Used to determine how many significant figures @p T has.
 *
 *  @tparam T The type to check. Must satisfy the UnmodifiedFloatingPoint
 *            concept.
 *
 *  The goal of this struct is to work out how many significant digits an object
 *  of type @p T can represent (in base 10). This is useful for trying to
 *  minimize precision loss in conversions among floating-point types.
 *
 *  This struct will contain a member `value` that is equivalent to
 *  std::numeric_limits<T>::digits10 when @p T is a type
 *  for which std::numeric_limits is specialized and 0 otherwise.
 *
 *  Users may specialize this class for their own types if they wish to have
 *  the precision detected correctly.
 */
template<concepts::UnmodifiedFloatingPoint T>
struct Precision {
public:
    /// Type used for measuring the precision
    using size_type = std::size_t;

private:
    /// Gets value from std::numeric_limits if it is specialized, 0 otherwise
    static constexpr size_type value_() {
        if constexpr(std::numeric_limits<T>::is_specialized) {
            return std::numeric_limits<T>::digits10;
        } else {
            return 0;
        }
    }

public:
    /// Number of significant digits in base 10 that can be represented by T
    constexpr static size_type value = value_();
};

/** @brief Global variable with the precision of an object of type @p T.
 *
 *  @tparam T The type to check. Must satisfy the UnmodifiedFloatingPoint
 *            concept.
 *
 *  This is just a convenient shorthand for Precision<T>::value. See the
 *  documentation for Precision for more details.
 */
template<concepts::UnmodifiedFloatingPoint T>
constexpr auto precision_v = Precision<T>::value;

} // namespace wtf::type_traits
