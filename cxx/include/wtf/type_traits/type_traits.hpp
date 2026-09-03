/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <wtf/enums/detail_/kind_dispatch.hpp>
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/is_convertible.hpp>
#include <wtf/type_traits/is_floating_point.hpp>
#include <wtf/type_traits/precision.hpp>
#include <wtf/type_traits/tuple_append.hpp>
#include <wtf/type_traits/type_name.hpp>

/** @brief Macro for quickly registering a @p T for use with WTF.
 *
 *  This macro should be used to register a custom floating-point type @p T
 *  with WTF. "Custom" in this context means any type for which
 *  C++20's std::is_floating_point<T> is false.
 *
 *  @param[in] T The unqualified type to be registered.
 *
 *  In addition to specializing IsFloatingPoint<T> and TypeName<T>, this
 *  macro registers a wtf::enums::FloatKind enumerator for @p T (see
 *  wtf::enums::float_kind), so `wtf::enums::from_string(#T)` works without
 *  any further action from the caller.
 *
 *  @note This macro should be used in the global namespace.
 *  @note This macro should be used only once per type.
 *  @note This macro will not work if T is a template.
 *  @note The FloatKind registration happens as the side effect of
 *        initializing a static variable. If the translation unit invoking
 *        this macro is linked into a static library and nothing else in
 *        that TU is referenced by the final binary, the linker may drop the
 *        TU (and thus the registration) entirely. If that happens, call
 *        wtf::enums::float_kind<T>() explicitly once at startup instead.
 */
#define WTF_REGISTER_FP_TYPE(T)                                              \
    namespace wtf::type_traits {                                             \
    template<>                                                               \
    struct IsFloatingPoint<T> : std::true_type {};                           \
    template<>                                                               \
    struct TypeName<T> {                                                     \
        static constexpr const char* value = #T;                             \
    };                                                                       \
    } /* namespace wtf::type_traits */                                       \
    namespace wtf::enums::detail_ {                                          \
    static const bool WTF_ENUMS_DETAIL_CONCAT(wtf_float_kind_auto_register_, \
                                              __COUNTER__) =                 \
      wtf::enums::detail_::register_float_kind_<T>();                        \
    } /* namespace wtf::enums::detail_ */

/// @brief Namespace for type traits provided by WTF.
namespace wtf::type_traits {}
