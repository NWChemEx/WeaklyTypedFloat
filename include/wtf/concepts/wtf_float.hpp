#pragma once
#include <type_traits>
#include <wtf/forward.hpp>

namespace wtf::concepts {

/** @brief Determines if @p T is a wtf::Float object.
 *
 *  @tparam T The type to check.
 */
template<typename T>
concept WTFFloat =
  std::is_same_v<T, fp::Float> || std::is_same_v<T, const fp::Float>;

} // namespace wtf::concepts