#pragma once
#include <type_traits>
#include <wtf/concepts/floating_point.hpp>

namespace wtf::type_traits {

/** @brief Can an object of type @p From be implicitly converted to type @p To
 *
 *  @tparam From The type being converted from. Must satisfy the
 *               wtf::FloatingPoint concept.
 *  @tparam To The type being converted to. Must satisfy the
 *              wtf::FloatingPoint concept.
 *
 *  This trait determines if an object of type @p From can be implicitly
 *  converted to an object of type @p To. This is determined by relying on the
 *  std::is_convertible type trait from the C++ standard library. If @p From can
 *  be implicitly converted to @p To then the static member @p value will be
 *  set to true, otherwise it will be false.
 */
template<concepts::UnmodifiedFloatingPoint From,
         concepts::UnmodifiedFloatingPoint To>
struct IsConvertible {
    static constexpr bool value = std::is_convertible_v<From, To>;
};

template<concepts::UnmodifiedFloatingPoint From,
         concepts::UnmodifiedFloatingPoint To>
inline constexpr bool is_convertible_v = IsConvertible<From, To>::value;

} // namespace wtf::type_traits