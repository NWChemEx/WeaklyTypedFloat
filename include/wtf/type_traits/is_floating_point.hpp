#pragma once
#include <type_traits>

namespace wtf::type_traits {

/** @brief WTF's trait for determining if @p T is a floating point type.
 *
 *  @param T The type to check.
 *
 *  While the C++ standard library provides `std::is_floating_point`, users
 *  should not extend it. This trait serves as a wrapper around
 *  `std::is_floating_point` that users can safely specialize in order to
 *  register new floating point types with WTF.
 *
 *  The primary template contains a static boolean member `value` that is set
 *  to true if `std::is_floating_point_v<T>` is true, and false otherwise.
 */ 
template <typename T> struct IsFloatingPoint {
  constexpr static bool value = std::is_floating_point_v<T>;
};

/// Convenience variable template for grabbing `IsFloatingPoint<T>::value`
template <typename T>
constexpr bool is_floating_point_v = IsFloatingPoint<T>::value;

} // namespace wtf::type_traits