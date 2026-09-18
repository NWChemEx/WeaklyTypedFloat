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
#include <utility>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/dispatcher.hpp>

namespace wtf::cast::detail_ {

/** @brief Downcasts @p args to `ModelType<T>` (for whichever `T` in
 *         @p TupleType actually matches) and calls @p visitor with each
 *         model's `handle()`.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the caller.
 *  @tparam TupleType A std::tuple of candidate floating-point types to try.
 *                    Must be provided by the caller.
 *  @tparam Visitor The type of the visitor to call. Will be inferred by the
 *                 compiler.
 *  @tparam Args The types of the holders to downcast. Will be inferred by
 *              the compiler.
 *
 *  This function generalizes the near-identical `visit_float_model`/
 *  `visit_float_view_model`/`visit_contiguous_model`/
 *  `visit_contiguous_view_model` wrappers (each of which built a lambda
 *  calling `.data()`/`.span()` and forwarded to
 *  `wtf::detail_::dispatch<ModelType, TupleType>`) into one shared
 *  implementation, per the proposal in `docs/source/developer/kokkos.rst`.
 *  Every Model class exposes a uniformly-named `handle()` accessor for "the
 *  thing you iterate" -- a reference to the held scalar for the `fp/`-side
 *  models, a `std::span` for the `buffer/`-side ones -- so this function
 *  itself does not need to know which domain it is being used for.
 *
 *  @param[in] visitor The visitor to call with the unwrapped handles.
 *  @param[in] args The holders to downcast and pass to the visitor.
 *
 *  @return The result of invoking @p visitor with the unwrapped handles.
 *
 *  @throw std::runtime_error if any of @p args cannot be downcast to a
 *                            `ModelType<T>` holding one of the types in
 *                            @p TupleType. Strong throw guarantee.
 *  @throw ??? if calling @p visitor throws. Same throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TupleType, typename Visitor, typename... Args>
auto visit_as(Visitor&& visitor, Args&&... args) {
    auto lambda = [&](auto&&... a) { return visitor(a.handle()...); };
    return wtf::detail_::dispatch<ModelType, TupleType>(
      lambda, std::forward<Args>(args)...);
}

} // namespace wtf::cast::detail_
