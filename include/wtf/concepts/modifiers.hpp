#pragma once
#include <type_traits>

namespace wtf::concepts {

/** @brief Determines if @p T is a pointer.
 *
 *  @param T The type to check.
 *
 *  This concept is satisfied if @p T is a pointer type regardless of any other
 *  type qualifiers that may or may not be present.
 */
template<typename T>
concept IsPointer = std::is_pointer_v<T>;

/** @brief Determines if @p T is a reference.
 *
 *  @param T The type to check.
 *
 *  This concept is satisfied if @p T is a reference type regardless of any
 *  other type qualifiers that may or may not be present.
 */
template<typename T>
concept IsReference = std::is_reference_v<T>;

/** @brief Determines if @p T is const-qualified.
 *
 *  @param T The type to check.
 *
 *  This concept is satisfied if @p T is const-qualified regardless of any other
 *  type qualifiers that may or may not be present. Notably this differs from
 *  `std::is_const_v` in that it will return true for pointer and reference
 *  types.
 */
template<typename T>
concept ConstQualified =
  std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>;

/** @brief Determines if @p T is volatile-qualified.
 *
 *  @param T The type to check.
 *
 *  This concept is satisfied if @p T is volatile-qualified regardless of any
 *  other type qualifiers that may or may not be present. Notably this differs
 *  from `std::is_volatile_v` in that it will return true for pointer and
 *  reference types.
 */
template<typename T>
concept VolatileQualified =
  std::is_volatile_v<std::remove_reference_t<std::remove_pointer_t<T>>>;

/** @brief Determines if @p T is an unqualified type.
 *
 *  @param T The type to check.
 *
 *  AFAIK C++ does not have official language for the absence of type modifiers.
 *  We'll use "unmodified" to mean the absence of type modifiers like const,
 *  pointer, or reference modifiers.
 *
 *  This concept is satisfied if @p T is not cv-qualified, not a pointer,
 *  and not a reference.
 */
template<typename T>
concept Unmodified = !ConstQualified<T> && !VolatileQualified<T> &&
                     !IsPointer<T> && !IsReference<T>;

} // namespace wtf::concepts