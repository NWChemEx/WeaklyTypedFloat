#pragma once
#include <wtf/type_traits/is_floating_point.hpp>

namespace wtf {

/// Establishes that T is the type of floating point value
template<typename T>
concept FloatingPoint = wtf::type_traits::is_floating_point_v<T>;

/// Establishes that T is a const-qualified floating point type
template<typename T>
concept ConstFloatingPoint =
  FloatingPoint<std::decay_t<T>> && std::is_const_v<T>;

/// Establishes that T is a volatile-qualified floating point type
template<typename T>
concept VolatileFloatingPoint =
  FloatingPoint<std::decay_t<T>> && std::is_volatile_v<T>;

/// Establishes that T is a floating point type that may also have type
/// qualifiers
template<typename T>
concept QualifiedFloatingPoint =
  FloatingPoint<T> || ConstFloatingPoint<T> || VolatileFloatingPoint<T>;

} // namespace wtf