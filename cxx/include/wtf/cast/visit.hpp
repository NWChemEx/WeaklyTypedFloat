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
#include <type_traits>
#include <utility>
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/buffer/float_buffer.hpp>
#include <wtf/fp/float.hpp>
#include <wtf/fp/float_view.hpp>

namespace wtf::cast::detail_ {

/// True if @p T (decayed) is some `fp::FloatView<X>` specialization.
template<typename T>
struct is_float_view : std::false_type {};
template<typename X>
struct is_float_view<fp::FloatView<X>> : std::true_type {};
template<typename T>
inline constexpr bool is_float_view_v = is_float_view<std::decay_t<T>>::value;

/// True if @p T (decayed) is some `buffer::BufferView<X>` specialization.
template<typename T>
struct is_buffer_view : std::false_type {};
template<typename X>
struct is_buffer_view<buffer::BufferView<X>> : std::true_type {};
template<typename T>
inline constexpr bool is_buffer_view_v = is_buffer_view<std::decay_t<T>>::value;

} // namespace wtf::cast::detail_

namespace wtf::cast {

/** @brief Visits zero or more `wtf::Float` objects with their type-restored
 *         values.
 *
 *  @related wtf::fp::Float
 *
 *  This is `wtf::cast`'s unified name for `wtf::fp::visit_float`, given the
 *  same name regardless of whether @p args are `Float`, `FloatView`,
 *  `FloatBuffer`, or `BufferView` objects (see the other overloads below),
 *  so a caller does not need to remember which domain-specific function
 *  name to reach for.
 *
 *  @tparam TupleType A std::tuple of floating-point types to try. Must be
 *                    provided by the caller.
 *  @tparam Visitor The type of the visitor to call. Will be inferred by the
 *                 compiler.
 *  @tparam Args The Float objects to type-restore. Will be inferred by the
 *              compiler.
 *
 *  @param[in] visitor The visitor to call with the type-restored values.
 *  @param[in] args The Float objects to type-restore and pass to the
 *                  visitor.
 *
 *  @return The result of invoking @p visitor with the type-restored values.
 *
 *  @throw std::runtime_error if none of the types in @p TupleType match the
 *                            type held by @p args. Strong throw guarantee.
 */
template<typename TupleType, typename Visitor, typename... Args>
    requires(std::same_as<std::decay_t<Args>, fp::Float> && ...)
auto visit(Visitor&& visitor, Args&&... args) {
    return fp::visit_float<TupleType>(std::forward<Visitor>(visitor),
                                      std::forward<Args>(args)...);
}

/** @brief Visits zero or more `wtf::FloatView` objects with their
 *         type-restored aliased values.
 *
 *  @related wtf::fp::FloatView
 *
 *  See `visit(Visitor&&, Args&&...)` above; this is the `FloatView` overload
 *  of the same unified name, forwarding to `wtf::fp::visit_float_view`.
 */
template<typename TupleType, typename Visitor, typename... Args>
    requires(detail_::is_float_view_v<Args> && ...)
auto visit(Visitor&& visitor, Args&&... args) {
    return fp::visit_float_view<TupleType>(std::forward<Visitor>(visitor),
                                           std::forward<Args>(args)...);
}

/** @brief Visits zero or more `wtf::buffer::FloatBuffer` objects with their
 *         type-restored contiguous storage.
 *
 *  @related wtf::buffer::FloatBuffer
 *
 *  See `visit(Visitor&&, Args&&...)` above; this is the `FloatBuffer`
 *  overload of the same unified name, forwarding to
 *  `wtf::buffer::visit_contiguous_buffer`.
 */
template<typename TupleType, typename Visitor, typename... Args>
    requires(std::same_as<std::decay_t<Args>, buffer::FloatBuffer> && ...)
auto visit(Visitor&& visitor, Args&&... args) {
    return buffer::visit_contiguous_buffer<TupleType>(
      std::forward<Visitor>(visitor), std::forward<Args>(args)...);
}

/** @brief Visits zero or more `wtf::buffer::BufferView` objects with their
 *         type-restored aliased contiguous storage.
 *
 *  @related wtf::buffer::BufferView
 *
 *  See `visit(Visitor&&, Args&&...)` above; this is the `BufferView`
 *  overload of the same unified name, forwarding to
 *  `wtf::buffer::visit_contiguous_buffer_view`.
 */
template<typename TupleType, typename Visitor, typename... Args>
    requires(detail_::is_buffer_view_v<Args> && ...)
auto visit(Visitor&& visitor, Args&&... args) {
    return buffer::visit_contiguous_buffer_view<TupleType>(
      std::forward<Visitor>(visitor), std::forward<Args>(args)...);
}

} // namespace wtf::cast
