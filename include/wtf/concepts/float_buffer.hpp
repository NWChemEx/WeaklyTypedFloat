#pragma once
#include <type_traits>
#include <wtf/forward.hpp>

namespace wtf::concepts {

/** @brief Determines if @p T is a wtf::buffer::FloatBuffer object.
 *
 *  @tparam T The type to check.
 */
template<typename T>
concept FloatBuffer = std::is_same_v<T, buffer::FloatBuffer> ||
                      std::is_same_v<T, const buffer::FloatBuffer>;

} // namespace wtf::concepts