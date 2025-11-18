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

namespace wtf::type_traits {

/** @brief Primary template for appending @p Tuple1 to @p Tuple0.
 *
 *  @tparam Tuple0 A std::tuple of 0 or more types.
 *  @tparam Tuple1 A std::tuple of 0 or more types.
 *
 *  The primary template is not defined. Instead this struct is specialized for
 *  the case where both @p Tuple0 and @p Tuple1 are std::tuples. By specializing
 *  like this we get access to the parameter packs inside the tuples and can
 *  append them together.
 */
template<typename Tuple0, typename Tuple1>
struct TupleAppend;

/** @brief Specializes TupleAppend to two tuples.
 *
 *  @tparam Args0 The types inside @p Tuple0.
 *  @tparam Args1 The types inside @p Tuple1.
 *
 *  This specialization implements TupleAppend for the case where both template
 *  type parameters are std::tuples. It unpacks the types of each tuple into
 *  a new tuple by appending the second tuple's types to the first tuple's
 *  types (we avoid calling this concatenation because tuple_cat is part of the
 *  std library and does something slightly different). The type of the
 *  resulting tuple is available as the member type `type`.
 */
template<typename... Args0, typename... Args1>
struct TupleAppend<std::tuple<Args0...>, std::tuple<Args1...>> {
    /// Type that is the result of appending Tuple1 to Tuple0
    using type = std::tuple<Args0..., Args1...>;
};

/** @brief Facilitates accessing TupleAppend::type.
 *
 *  @tparam Tuple0 A std::tuple of 0 or more types.
 *  @tparam Tuple1 A std::tuple of 0 or more types.
 *
 *  This alias template is a convenience for accessing the member `type` of
 *  TupleAppend.
 */
template<typename Tuple0, typename Tuple1>
using tuple_append_t = typename TupleAppend<Tuple0, Tuple1>::type;

} // namespace wtf::type_traits
