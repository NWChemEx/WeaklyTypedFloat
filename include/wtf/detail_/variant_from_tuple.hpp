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
#include <tuple>
#include <variant>
#include <wtf/concepts/floating_point.hpp>

namespace wtf::detail_ {

/** @brief Primary template for creating a std::variant of ModelType types.
 *
 *  @tparam ModelType A template template parameter that takes a type
 *                    satisfying QualifiedFloatingPoint.
 *  @tparam T         A type that is expected to be a std::tuple of types
 *                    satisfying FloatingPoint concept.
 *
 *  The primary template is undefined. A specialization is provided for when
 *  @p T is a std::tuple. The specialization is required to get at the types
 *  in the tuple. For each type U in @p T, the specialization should provide
 *  four member types:
 *  - value:   A std::variant of ModelType<U>* types
 *  - cvalue:  A std::variant of ModelType<const U>* types
 *  - vvalue:  A std::variant of ModelType<volatile U>* types
 *  - cvvalue: A std::variant of ModelType<const volatile U>* types
 */
template<template<concepts::FloatingPoint> typename ModelType, typename T>
struct VariantFromTuple;

/** @brief Specializes primary template to std::tuple.
 *
 *  @tparam ModelType A template template parameter that takes a type satisfying
 *                    QualifiedFloatingPoint.
 *  @tparam Args      A parameter pack of types that satisfy the FloatingPoint
 *                    concept.
 *
 *  This specialization uses the fact that it can get at the parameter pack of
 *  types in the std::tuple. More specifically, for each type T in Args, it
 *  implements the four member types called for in the primary template by
 *  applying the necessary cv-qualifications to T.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         concepts::UnmodifiedFloatingPoint... Args>
struct VariantFromTuple<ModelType, std::tuple<Args...>> {
    /// How each ModelType is represented in the variant
    template<concepts::FloatingPoint T>
    using model_type = ModelType<T>*;

    /// A variant where the ModelType's types are unqualified
    using value = std::variant<model_type<Args>...>;

    /// A variant where the ModelType's types are const-qualified
    using cvalue = std::variant<model_type<const Args>...>;
};

/** @brief Primary template for creating a std::variant of const ModelType types
 *
 *  @tparam ModelType A template template parameter that takes a type that
 *                    satisfies the QualifiedFloatingPoint concept.
 *  @tparam T         A type that is expected to be a std::tuple of types.
 *
 *  This template is like VariantFromTuple, but the contained types are all
 *  const-qualified ModelType types. See the description for the
 *  VariantFromTuple primary template for more details.
 */
template<template<concepts::FloatingPoint> typename ModelType, typename T>
struct ConstVariantFromTuple;

/** @brief Implements ConstVariantFromTuple for std::tuple.
 *
 *  @tparam ModelType A template template parameter that takes a type that
 *                    satisfies the QualifiedFloatingPoint concept.
 *  @tparam Args      A parameter pack of types that satisfy the FloatingPoint
 *                    concept.
 *
 *  This specialization is like the VariantFromTuple specialization, but results
 *  in each of the ModelType types to be const-qualified. See the description
 *  for the VariantFromTuple specialization for more details.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         concepts::UnmodifiedFloatingPoint... Args>
struct ConstVariantFromTuple<ModelType, std::tuple<Args...>> {
    /// How each ModelType is represented in the variant
    template<concepts::FloatingPoint T>
    using model_type = const ModelType<T>*;

    /// A variant where the ModelType's types are unqualified
    using value = std::variant<model_type<Args>...>;

    /// A variant where the ModelType's types are const-qualified
    using cvalue = std::variant<model_type<const Args>...>;
};

} // namespace wtf::detail_
