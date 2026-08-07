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
#include <wtf/detail_/downcaster.hpp>

namespace wtf::detail_ {

/// Primary template for concatenating the alternatives of two variants.
template<typename VariantA, typename VariantB>
struct variant_cat;

/** @brief Concatenates the alternatives of two std::variant types.
 *
 *  @tparam As The alternatives of the first variant.
 *  @tparam Bs The alternatives of the second variant.
 */
template<typename... As, typename... Bs>
struct variant_cat<std::variant<As...>, std::variant<Bs...>> {
    /// A std::variant holding the alternatives of both input variants.
    using type = std::variant<As..., Bs...>;
};

/// Convenience alias for `typename variant_cat<VariantA, VariantB>::type`.
template<typename VariantA, typename VariantB>
using variant_cat_t = typename variant_cat<VariantA, VariantB>::type;

/** @brief Downcasts a single holder to a variant that can hold either the
 *         mutable or const-qualified result.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the user.
 *  @tparam TypeTuple A std::tuple of floating point types that ModelType can
 *                    wrap. Must be provided by the user.
 *  @tparam T The type of the holder to downcast. Will be inferred by the
 *            compiler.
 *
 *  Whether @p holder is const-qualified (as determined at runtime by
 *  `holder.is_const()`) selects between two disjoint variant types, namely
 *  the one returned by downcaster and the one returned by const_downcaster.
 *  This function flattens both possibilities into a single variant type so
 *  that the resulting variants for each holder passed to dispatch can be
 *  visited together in a single std::visit call.
 *
 *  @param[in] holder The holder to downcast.
 *
 *  @return A variant holding a pointer to the downcasted type.
 *
 *  @throws std::runtime_error if the type cannot be found in the variant.
 *                             Strong throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TypeTuple, typename T>
auto downcast_arg(T&& holder) {
    using mutable_variant_t =
      decltype(downcaster<ModelType, TypeTuple>(holder));
    using const_variant_t =
      decltype(const_downcaster<ModelType, TypeTuple>(holder));
    using flat_variant_t = variant_cat_t<mutable_variant_t, const_variant_t>;

    auto flatten = [](auto* p) -> flat_variant_t { return flat_variant_t{p}; };

    // Because we must call the correct downcaster based on cv-qualifiers that
    // are only knowable at runtime, and because those downcasters all have
    // different return types (and thus signatures) we must explicitly branch.
    if(holder.is_const()) {
        auto v =
          const_downcaster<ModelType, TypeTuple>(std::forward<T>(holder));
        return std::visit(flatten, v);
    } else {
        auto v = downcaster<ModelType, TypeTuple>(std::forward<T>(holder));
        return std::visit(flatten, v);
    }
}

/** @brief Wraps dispatching to a function based on the types contained in a
 *         series of holders.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the user.
 *  @tparam TypeTuple A std::tuple of floating point types that ModelType can
 *                    wrap. Must be provided by the user.
 *  @tparam FxnType The type of the callable to invoke. Will be inferred by the
 *                  compiler.
 *  @tparam HolderTypes The types of the holders to downcast. Will be inferred
 *                      by the compiler.
 *
 *  This is the high-level developer interface for dispatching based on the
 *  types contained in a series of holders. The user of this function provides
 *  the ModelType and TypeTuple template parameters, a callable, and a series of
 *  holders. This function will downcast each holder to a ModelType wrapping one
 *  of the types in TypeTuple. Once all holders have been downcasted the user's
 *  callable will be invoked with the downcasted models and the result will be
 *  returned.
 *
 *  @param[in] fxn The callable to invoke.
 *  @param[in] holders The holders to downcast.
 *
 *  @return The return value of the callable.
 *
 *  @throws ??? Any exception thrown by the callable or by downcast_arg. Same
 *              throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TypeTuple, typename FxnType, typename... HolderTypes>
auto dispatch(FxnType&& fxn, HolderTypes&&... holders) {
    if constexpr(sizeof...(HolderTypes) == 0) {
        return fxn();
    } else {
        // Braced-init-list guarantees left-to-right evaluation, preserving
        // the old recursion's deterministic "leftmost failing holder
        // throws" behavior (a bare pack-expanded call-argument list does
        // not guarantee this).
        std::tuple<decltype(downcast_arg<ModelType, TypeTuple>(
          std::declval<HolderTypes&&>()))...>
          variants{downcast_arg<ModelType, TypeTuple>(
            std::forward<HolderTypes>(holders))...};

        auto deref_and_call = [&fxn](auto*... ps) { return fxn(*ps...); };

        return std::apply(
          [&](auto&&... vs) { return std::visit(deref_and_call, vs...); },
          std::move(variants));
    }
}

} // namespace wtf::detail_
