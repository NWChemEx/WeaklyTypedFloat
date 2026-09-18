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
#include <vector>
#include <wtf/buffer/buffer_view.hpp>
#include <wtf/fp/float_view.hpp>
#include <wtf/types.hpp>

namespace wtf::cast {

/** @brief Policy-driven, always-by-value conversion of a `wtf::Float` (or
 *         `wtf::FloatView`) to a requested type.
 *
 *  @related wtf::fp::Float
 *
 *  This is `wtf::cast`'s unified name for `wtf::fp::convert_to`, given the
 *  same name regardless of whether the argument is a `Float`/`FloatView` or
 *  a `FloatBuffer`/`BufferView` (see the other overload below), so a caller
 *  does not need to remember which domain-specific function name to reach
 *  for. See `docs/source/developer/conversion.rst` for the full design.
 *
 *  @tparam T The type to convert the held value to.
 *  @tparam Policy The conversion policy to use. Defaults to
 *                 `fp::policies::Match` (only exact-type conversions).
 *  @tparam TupleType A std::tuple of candidate floating-point types to try.
 *                    Defaults to wtf::default_fp_types.
 *
 *  @param[in] f The Float (or FloatView) to convert.
 *
 *  @return The converted value.
 *
 *  @throw std::runtime_error if @p f does not hold one of the types in
 *                            @p TupleType, or if @p Policy declines to
 *                            convert the held value to @p T. Strong throw
 *                            guarantee.
 */
template<typename T, typename Policy = fp::policies::Match,
         typename TupleType = wtf::default_fp_types>
T convert(fp::FloatView<const fp::Float> f) {
    return fp::convert_to<T, Policy, TupleType>(f);
}

/** @brief Policy-driven, always-by-value conversion of a
 *         `wtf::buffer::FloatBuffer` (or `wtf::buffer::BufferView`) to a
 *         `std::vector` of a requested type.
 *
 *  @related wtf::buffer::FloatBuffer
 *
 *  See `convert(fp::FloatView<const fp::Float>)` above; this is the
 *  `FloatBuffer`/`BufferView` overload of the same unified name, forwarding
 *  to `wtf::buffer::convert_to`.
 */
template<typename T, typename Policy = fp::policies::Match,
         typename TupleType = wtf::default_fp_types>
std::vector<T> convert(buffer::BufferView<const fp::Float> buffer) {
    return buffer::convert_to<T, Policy, TupleType>(buffer);
}

} // namespace wtf::cast
