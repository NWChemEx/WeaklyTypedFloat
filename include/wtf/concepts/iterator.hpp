#pragma once
#include <iterator>
#include <wtf/concepts/floating_point.hpp>

namespace wtf::concepts {

/** @brief Determines if @p T is an iterator over a floating-point container.
 *
 *  @tparam T The type to check.
 */
template<typename T>
concept FPIterator =
  FloatingPoint<typename std::iterator_traits<T>::value_type>;

} // namespace wtf::concepts