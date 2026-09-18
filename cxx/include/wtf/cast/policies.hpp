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
#include <concepts>
#include <optional>
#include <type_traits>
#include <wtf/type_traits/precision.hpp>

namespace wtf::cast::policies {

/** @brief The default `convert_to` policy: only exact-type conversions.
 *
 *  `Match` is engaged only when the held type and the requested type are the
 *  same (up to cv-qualification). This is the same check `float_cast<T>`
 *  already performs; the only difference is that `convert_to` with `Match`
 *  always returns a copy, never a reference.
 */
struct Match {
    template<typename T, typename U>
    static std::optional<T> convert(U&& value) {
        if constexpr(std::same_as<std::decay_t<U>, std::decay_t<T>>) {
            return static_cast<T>(value);
        } else {
            return std::nullopt;
        }
    }
};

/** @brief A `convert_to` policy allowing any implicit conversion.
 *
 *  `Convertible` is engaged whenever the held type is convertible to the
 *  requested type, including narrowing conversions that may lose precision
 *  (e.g. `double` to `float`). See `Widen` for a precision-safe alternative.
 */
struct Convertible {
    template<typename T, typename U>
    static std::optional<T> convert(U&& value) {
        if constexpr(std::convertible_to<std::decay_t<U>, T>) {
            return static_cast<T>(value);
        } else {
            return std::nullopt;
        }
    }
};

/** @brief A `convert_to` policy allowing only conversions that can not lose
 *         precision.
 *
 *  `Widen` is engaged only when the held type is convertible to the
 *  requested type AND the requested type has at least as many significant
 *  digits as the held type, per `wtf::type_traits::Precision`. This allows,
 *  e.g., `float` to `double`, but rejects `double` to `float`.
 */
struct Widen {
    template<typename T, typename U>
    static std::optional<T> convert(U&& value) {
        using clean_u = std::decay_t<U>;
        if constexpr(std::convertible_to<clean_u, T> &&
                     type_traits::precision_v<T> >=
                       type_traits::precision_v<clean_u>) {
            return static_cast<T>(value);
        } else {
            return std::nullopt;
        }
    }
};

} // namespace wtf::cast::policies
