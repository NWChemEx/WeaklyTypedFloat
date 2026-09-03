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
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <wtf/enums/float_kind.hpp>

/// @{
/// Token-pastes @p a and @p b into a single, unique preprocessor identifier.
/// Used by WTF_REGISTER_FP_TYPE (see wtf/type_traits/type_traits.hpp) to name
/// the static object that auto-registers a type's FloatKind.
#define WTF_ENUMS_DETAIL_CONCAT_IMPL(a, b) a##b
#define WTF_ENUMS_DETAIL_CONCAT(a, b) WTF_ENUMS_DETAIL_CONCAT_IMPL(a, b)
/// @}

namespace wtf::enums::detail_ {

/** @brief Forces registration of @p T's FloatKind.
 *
 *  @tparam T The floating-point type to register.
 *
 *  This function exists so that WTF_REGISTER_FP_TYPE can register a type's
 *  FloatKind as the side effect of initializing a static bool (see that
 *  macro), without needing to spell out float_kind<T>()'s return type (which
 *  would otherwise trigger an unused-result warning).
 *
 *  @return Always true.
 */
template<typename T>
bool register_float_kind_() {
    float_kind<T>();
    return true;
}

/** @brief Calls @p fxn with the type in @p TupleType identified by @p kind.
 *
 *  @tparam TupleType A std::tuple of candidate floating-point types. Must be
 *                    provided by the caller.
 *  @tparam Fxn The type of the callable to invoke. Must be a generic,
 *              templated callable, i.e. it must define a templated
 *              `operator()<T>()`. Will be inferred by the compiler.
 *
 *  This function is the runtime counterpart to picking one of the types in
 *  @p TupleType at compile time: it walks @p TupleType looking for the type
 *  T for which `float_kind<T>() == kind`, and, upon finding it, invokes
 *  `fxn.template operator()<T>()` and returns the result.
 *
 *  @param[in] kind The FloatKind identifying which type in @p TupleType to
 *                  invoke @p fxn with.
 *  @param[in] fxn The callable to invoke with the identified type.
 *
 *  @return The result of invoking @p fxn with the type identified by
 *          @p kind.
 *
 *  @throw std::runtime_error if none of the types in @p TupleType are
 *                            identified by @p kind. Strong throw guarantee.
 *  @throw ??? if @p fxn throws. Same throw guarantee.
 */
template<typename TupleType, typename Fxn>
auto dispatch_by_kind(FloatKind kind, Fxn&& fxn) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        using result_t =
          decltype(fxn.template
                   operator()<std::tuple_element_t<0, TupleType>>());
        std::optional<result_t> result;

        auto try_one = [&]<typename T>() {
            if(kind != float_kind<T>()) return false;
            result.emplace(fxn.template operator()<T>());
            return true;
        };

        bool found =
          (try_one.template operator()<std::tuple_element_t<Is, TupleType>>() ||
           ...);
        if(!found) {
            throw std::runtime_error("FloatKind '" + to_string(kind) +
                                     "' is not among the candidate types");
        }
        return std::move(*result);
    }(std::make_index_sequence<std::tuple_size_v<TupleType>>{});
}

} // namespace wtf::enums::detail_
