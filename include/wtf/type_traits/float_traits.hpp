#pragma once
#include <wtf/concepts/floating_point.hpp>

namespace wtf::type_traits {

/** @brief Works out the types associated with @p T.
 *
 *  @tparam T A floating-point type, possibly with cv-qualifiers.
 *
 *  Throughout WTF we will need more than just types like float, double, etc.
 *  We will also need references, pointers, and cv-qualified versions. Rather
 *  than assume that T is always a plain floating-point type, we use this traits
 *  class to do the conversion for us. Of note, this means that users can
 *  specialize the class for custom floating-point classes when those classes
 *  have custom reference/pointer classes.
 */
template<concepts::FloatingPoint T>
struct float_traits {
    /// The type of an object declared with type @p T
    using value_type = T;

    /// The type of a const-qualified object declared with type @p T
    using const_value_type = const value_type;

    /// Type acting like a reference to an object of value_type
    using reference = value_type&;

    /// Type acting like a const reference to an object of value_type
    using const_reference = const value_type&;

    /// Type acting like a pointer to an object of value_type
    using pointer = value_type*;

    /// Type acting like a const pointer to an object of value_type
    using const_pointer = const value_type*;

    /// True if value_type is const-qualified, false otherwise
    static constexpr bool is_const =
      std::is_const_v<std::remove_reference_t<T>>;
};

} // namespace wtf::type_traits