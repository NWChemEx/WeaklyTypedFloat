#pragma once
#include <type_traits>

namespace wtf::concepts {

template<typename T>
concept IsPointer = std::is_pointer_v<T>;

template<typename T>
concept IsReference = std::is_reference_v<T>;

template<typename T>
concept ConstQualified = std::is_const_v<T>;

template<typename T>
concept VolatileQualified = std::is_volatile_v<T>;

/** @brief Determines if @p T is an unqualified type.
 *
 *  @param T The type to check.
 *
 *  AFAIK C++ does not have official language for the absence of type modifiers.
 *  We'll use "unmodified" to mean the absence of type modifiers like const,
 *  pointer, or reference modifiers.
 *
 *  This concept is satisfied if @p T is not const-qualified, not a pointer,
 *  and not a reference.
 */
template<typename T>
concept Unmodified = !ConstQualified<T> && !VolatileQualified<T> &&
                     !IsPointer<T> && !IsReference<T>;

} // namespace wtf::concepts