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

namespace wtf::cast::detail_ {

/** @brief Downcasts @p holder to `ModelType<T>` or throws.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the caller.
 *  @tparam T The exact type @p holder is expected to hold. Must be provided
 *            by the caller (this function does not itself decay/adjust the
 *            constness of @p T -- callers are expected to have already
 *            computed whatever const-correct type they need).
 *  @tparam Holder The type of @p holder. Will be inferred by the compiler;
 *                 `Holder&` naturally becomes `const X&` when called on a
 *                 const holder, preserving the caller's const-correctness.
 *
 *  This function factors out the single-type "downcast this holder to
 *  `ModelType<T>`, or throw if it isn't actually holding a `T`" pattern that
 *  `wtf::fp::float_cast`, `wtf::fp::FloatView::value`,
 *  `wtf::buffer::FloatBuffer::value`, and `wtf::buffer::BufferView::value`
 *  each used to hand-roll independently.
 *
 *  @param[in,out] holder The holder to downcast.
 *  @param[in] msg The message to use for the thrown exception if @p holder
 *                 does not actually hold a `ModelType<T>`.
 *
 *  @return A reference to @p holder, downcast to `ModelType<T>`.
 *
 *  @throw std::runtime_error if @p holder does not hold a `ModelType<T>`.
 *                            Strong throw guarantee.
 */
template<template<typename> typename ModelType, typename T, typename Holder>
auto& restore(Holder& holder, const char* msg) {
    using model_type = ModelType<T>;
    auto* pmodel     = dynamic_cast<model_type*>(&holder);
    if(pmodel == nullptr) { throw std::runtime_error(msg); }
    return *pmodel;
}

} // namespace wtf::cast::detail_
