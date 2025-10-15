#pragma once
#include <wtf/concepts/modifiers.hpp>
#include <wtf/type_traits/is_floating_point.hpp>

namespace wtf::concepts {

/// Establishes that T is an unmodified type of floating point value
template<typename T>
concept UnmodifiedFloatingPoint =
  Unmodified<T> && wtf::type_traits::is_floating_point_v<T>;

/// Establishes that T is a floating point type, possibly with
/// const-qualification
template<typename T>
concept FloatingPoint = UnmodifiedFloatingPoint<std::remove_const_t<T>>;

/// Establishes that T is a const-qualified floating point type
template<typename T>
concept ConstFloatingPoint = FloatingPoint<T> && ConstQualified<T>;

} // namespace wtf::concepts