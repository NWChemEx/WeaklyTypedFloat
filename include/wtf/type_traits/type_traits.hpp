#pragma once
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/is_floating_point.hpp>

/** @brief Macro for quickly registering a @p T for use with WTF.
 *
 *  This macro should be used to register a custom floating-point type @p T
 *  with WTF. "Custom" in this context means any type for which
 *  C++20's std::is_floating_point<T> is false.
 *
 *  @param[in] T The unqualified type to be registered.
 *
 *  @note This macro should be used in the global namespace.
 *  @note This macro should be used only once per type.
 *  @note This macro will not work if T is a template.
 */
#define WTF_REGISTER_FP_TYPE(T)                    \
    namespace wtf::type_traits {                   \
    template<>                                     \
    struct IsFloatingPoint<T> : std::true_type {}; \
    } // namespace wtf::type_traits