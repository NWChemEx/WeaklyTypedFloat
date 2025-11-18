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
#include <wtf/detail_/variant_from_tuple.hpp>

namespace wtf::detail_ {

/** @brief Recursive body for downcasting.
 *
 *  @tparam I The current index in the variant we are trying to cast to.
 *  @tparam VariantType The variant type we are casting to.
 *  @tparam T The type of the object we are downcasting from.
 *
 *  This function tries to dynamically cast the holder to the @p I'th type in
 *  VariantType. If that fails, it recurses with I+1. If I reaches the number
 *  of types in VariantType, it throws. This function will always restore the
 *  holder to the model with the same type as it was declared with, i.e., this
 *  function will NOT do any implicit or explicit conversions.
 *
 *  @note This function preserves constness. If the holder is a const type, and
 *        none of the types in variant are const, the function will fail to
 *        find a conversion and throw.
 *
 *
 *  @throws std::runtime_error if the type cannot be found in the variant.
 *                             Strong throw guarantee.
 */
template<std::size_t I, typename VariantType, typename T>
VariantType downcast_impl(T&& holder) {
    // Is holder a read-only object (regardless of whether it is holding a
    // read-only type)?
    using clean_type               = std::remove_reference_t<T>;
    static constexpr bool is_const = std::is_const_v<clean_type>;

    if constexpr(I == std::variant_size_v<VariantType>) {
        throw std::runtime_error("Type not in variant");
    } else {
        // This iteration we try to cast to TargetType
        using TargetType = std::variant_alternative_t<I, VariantType>;

        // Is TargetType a read-only type?
        using clean_target_type            = std::remove_pointer_t<TargetType>;
        static constexpr bool const_target = std::is_const_v<clean_target_type>;

        if constexpr(is_const && !const_target) {
            // Can't cast const to non-const
            return downcast_impl<I + 1, VariantType>(std::forward<T>(holder));
        } else {
            if(auto* p = dynamic_cast<TargetType>(&holder)) {
                return VariantType{p};
            } else {
                return downcast_impl<I + 1, VariantType>(
                  std::forward<T>(holder));
            }
        }
    }
}

/** @brief Downcasts when holding a mutable type.
 *
 *  @tparam ModelType A template template parameters that takes a single
 *                    template parameter.
 *  @tparam Types A std::tuple filled with floating point types.
 *
 *  This function will call downcast_impl to do the actual downcasting. The
 *  function will preserve constness of the holder. If the holder is const-
 *  qualified when it is passed in, the returned variant will hold const-
 *  qualified pointers (and vice versa). This function will down cast to
 *  ModelType's where T is unqualified.
 *
 *  @note This function will NOT do any implicit or explicit conversions.
 *
 *  @return A variant holding a pointer to the downcasted type.
 *
 *  @throw std::runtime_error if the type cannot be found in the variant. Strong
 *                            throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType, typename Types,
         typename T>
auto downcaster(T&& holder) {
    constexpr bool is_const = std::is_const_v<std::remove_reference_t<T>>;
    using variant_t  = typename VariantFromTuple<ModelType, Types>::value;
    using cvariant_t = typename ConstVariantFromTuple<ModelType, Types>::value;
    using variant_type = std::conditional_t<is_const, cvariant_t, variant_t>;
    return downcast_impl<0, variant_type>(std::forward<T>(holder));
}

/** @brief Downcasts to variants holding const types.
 *
 *  @tparam ModelType A template template parameters that takes a single
 *                    template parameter.
 *  @tparam Types A std::tuple filled with floating point types.
 *
 *  This function is the same as downcaster, except that the ModelType's type
 *  will be const-qualified. See the documentation for downcaster for more
 *  details.
 *
 *  @return A variant holding a pointer to the downcasted type.
 *
 *  @throw std::runtime_error if the type cannot be found in the variant. Strong
 *                     throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType, typename Types,
         typename T>
auto const_downcaster(T&& holder) {
    constexpr bool is_const = std::is_const_v<std::remove_reference_t<T>>;
    using variant_t  = typename VariantFromTuple<ModelType, Types>::cvalue;
    using cvariant_t = typename ConstVariantFromTuple<ModelType, Types>::cvalue;
    using variant_type = std::conditional_t<is_const, cvariant_t, variant_t>;
    return downcast_impl<0, variant_type>(std::forward<T>(holder));
}

} // namespace wtf::detail_
