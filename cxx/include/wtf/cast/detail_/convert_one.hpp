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
#include <stdexcept>
#include <wtf/cast/detail_/visit_as.hpp>
#include <wtf/concepts/floating_point.hpp>

namespace wtf::cast::detail_ {

/** @brief Implements the policy-dispatch logic behind `convert_to`.
 *
 *  @tparam T The type to convert the held value to. Must be provided by the
 *            caller.
 *  @tparam Policy The conversion policy to use. Must expose
 *                 `static std::optional<T> convert<T, U>(U&& value)`.
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the caller.
 *  @tparam TupleType A std::tuple of candidate floating-point types to try.
 *  @tparam Holder The type of @p holder. Will be inferred by the compiler.
 *
 *  This function finds the one candidate `U` in @p TupleType whose
 *  `ModelType<U>` @p holder actually downcasts to (via `visit_as`, i.e. the
 *  same `wtf::detail_::dispatch` fold every other multi-candidate WTF
 *  dispatch already uses), then hands the restored value to
 *  `Policy::convert<T>`.
 *
 *  @param[in] holder The holder to restore and convert.
 *
 *  @return The converted value.
 *
 *  @throw std::runtime_error if @p holder does not hold one of the types in
 *                            @p TupleType, or if @p Policy declines to
 *                            convert the held value to @p T. Strong throw
 *                            guarantee.
 */
template<typename T, typename Policy,
         template<concepts::FloatingPoint> typename ModelType,
         typename TupleType, typename Holder>
T convert_one(Holder& holder) {
    auto lambda = [](auto&& handle) -> T {
        auto result = Policy::template convert<T>(handle);
        if(!result) { throw std::runtime_error("wtf::convert_to: bad cast"); }
        return *result;
    };
    return visit_as<ModelType, TupleType>(lambda, holder);
}

} // namespace wtf::cast::detail_
