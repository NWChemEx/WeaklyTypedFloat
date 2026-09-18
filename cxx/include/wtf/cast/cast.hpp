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
#include <span>
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/buffer/float_buffer.hpp>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/concepts/wtf_float.hpp>
#include <wtf/fp/float.hpp>
#include <wtf/fp/float_view.hpp>

namespace wtf::cast {

/** @brief Un-type-erases a `wtf::Float` to its exact held type.
 *
 *  @related wtf::fp::Float
 *
 *  This is `wtf::cast`'s unified name for `wtf::fp::float_cast`, given the
 *  same name regardless of whether the argument is a `Float`, a `FloatView`,
 *  a `FloatBuffer`, or a `BufferView` (see the other overloads below), so a
 *  caller does not need to remember which domain-specific function name to
 *  reach for.
 *
 *  @tparam T The type of floating-point value being extracted. Must satisfy
 *            the concepts::UnmodifiedFloatingPoint concept (up to type
 *            qualifiers).
 *
 *  @param[in] f The Float object to extract the value from.
 *
 *  @return The floating-point value held by @p f.
 *
 *  @throw std::runtime_error if @p f does not hold a value of type @p T.
 *                            Strong throw guarantee.
 */
template<typename T>
    requires concepts::UnmodifiedFloatingPoint<std::decay_t<T>>
T cast(fp::Float& f) {
    return fp::float_cast<T>(f);
}

/** @brief Un-type-erases a `wtf::FloatView` to its exact held type.
 *
 *  @related wtf::fp::FloatView
 *
 *  See `cast(fp::Float&)` above; this is the `FloatView` overload of the
 *  same unified name, forwarding to `wtf::fp::float_cast`.
 *
 *  @tparam T The type of floating-point value being extracted. Up to type
 *            qualifiers must satisfy the concepts::FloatingPoint concept.
 *  @tparam FloatType The type of wtf::Float being aliased by @p f. Will be
 *                    inferred by the compiler.
 *
 *  @param[in] f The FloatView to extract the value from.
 *
 *  @return The floating-point value aliased by @p f.
 *
 *  @throw std::runtime_error if @p f does not alias a value of type @p T.
 *                            Strong throw guarantee.
 */
template<typename T, concepts::WTFFloat FloatType>
    requires concepts::FloatingPoint<std::decay_t<T>>
T cast(fp::FloatView<FloatType> f) {
    return fp::float_cast<T>(f);
}

/** @brief Un-type-erases a `wtf::buffer::FloatBuffer` to a span of its exact
 *         held type.
 *
 *  @related wtf::buffer::FloatBuffer
 *
 *  See `cast(fp::Float&)` above; this is the `FloatBuffer` overload of the
 *  same unified name, forwarding to `wtf::buffer::contiguous_buffer_cast`.
 *
 *  @tparam T The type of floating-point value being extracted. Must satisfy
 *            the concepts::FloatingPoint concept.
 *
 *  @param[in] buffer The FloatBuffer to extract the span from.
 *
 *  @return A span aliasing the contiguous memory held by @p buffer.
 *
 *  @throw std::runtime_error if @p buffer is not contiguous, or does not
 *                            hold values of type @p T. Strong throw
 *                            guarantee.
 */
template<concepts::FloatingPoint T>
std::span<T> cast(buffer::FloatBuffer& buffer) {
    return buffer::contiguous_buffer_cast<T>(buffer);
}

/** @brief Un-type-erases a `wtf::buffer::BufferView` to a span of its exact
 *         held type.
 *
 *  @related wtf::buffer::BufferView
 *
 *  See `cast(fp::Float&)` above; this is the `BufferView` overload of the
 *  same unified name, forwarding to `wtf::buffer::contiguous_buffer_cast`.
 *
 *  @tparam T The type of floating-point value being extracted. Must satisfy
 *            the concepts::FloatingPoint concept.
 *  @tparam FloatType The type of buffer the BufferView is acting like. Will
 *                    be inferred by the compiler.
 *
 *  @param[in] buffer The BufferView to extract the span from.
 *
 *  @return A span aliasing the contiguous memory held by @p buffer.
 *
 *  @throw std::runtime_error if @p buffer is not contiguous, or does not
 *                            hold values of type @p T. Strong throw
 *                            guarantee.
 */
template<concepts::FloatingPoint T, concepts::WTFFloat FloatType>
std::span<T> cast(buffer::BufferView<FloatType>& buffer) {
    return buffer::contiguous_buffer_cast<T>(buffer);
}

} // namespace wtf::cast
