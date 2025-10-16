#pragma once
#include <wtf/concepts/floating_point.hpp>

namespace wtf::type_traits {

/** @brief Primary template for converting @p T to a string.
 *
 *  @tparam T A floating-point type without any type modifiers.
 *
 *  This is the primary template for mapping a type to a string. The primary
 *  template is not defined (we opted not to use typeid().name() because it is
 *  system-dependent and not very user friendly). Specializations are provided
 *  for C++'s built-in floating point types. User-defined floating point types
 *  can be supported by specializing this template for the user-defined type.
 */
template<concepts::UnmodifiedFloatingPoint T>
struct TypeName;

/// Specializes TypeName for float
template<>
struct TypeName<float> {
    static constexpr auto value = "float";
};

/// Specializes TypeName for double
template<>
struct TypeName<double> {
    static constexpr auto value = "double";
};

/// Specializes TypeName for long double
template<>
struct TypeName<long double> {
    static constexpr auto value = "long double";
};

} // namespace wtf::type_traits