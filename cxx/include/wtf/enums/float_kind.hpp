/*
 * Copyright 2026 NWChemEx-Project
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
#include <functional>
#include <string>
#include <type_traits>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/enums/detail_/float_kind_registry.hpp>
#include <wtf/type_traits/type_name.hpp>

namespace wtf::enums {

/** @brief An extensible, runtime enumerator identifying a registered
 *         floating-point type.
 *
 *  Unlike a plain `enum class`, the set of legal FloatKind values is not
 *  fixed when WTF is compiled. It starts out containing an enumerator for
 *  each type in wtf::default_fp_types, and grows every time
 *  WTF_REGISTER_FP_TYPE(T) registers a new type T with WTF (see
 *  float_kind()). FloatKind objects are cheap to copy/compare and are the
 *  runtime counterpart to specifying a floating-point type as a template
 *  parameter.
 *
 *  @note To create instances of this class, use the float_kind function (to
 *        get the FloatKind for a compile-time type) or the from_string
 *        function (to get the FloatKind registered under a given name).
 */
class FloatKind {
public:
    /// Type used to distinguish FloatKind objects from one another
    using id_type = detail_::FloatKindRegistry::id_type;

    /// FloatKind objects always identify a registered type, so no default
    FloatKind() = delete;

    /** @brief Determines if *this and @p other identify the same type.
     *
     *  @param[in] other The FloatKind to compare against *this.
     *
     *  @return True if *this and @p other identify the same registered type
     *          and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FloatKind& other) const noexcept {
        return m_id_ == other.m_id_;
    }

    /** @brief Determines if *this and @p other identify different types.
     *
     *  @param[in] other The FloatKind to compare against *this.
     *
     *  @return False if *this and @p other identify the same registered type
     *          and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const FloatKind& other) const noexcept {
        return !(*this == other);
    }

    /** @brief Establishes an (arbitrary, but consistent) order among
     *         FloatKind objects.
     *
     *  This operator is provided so that FloatKind objects can be used with
     *  ordered containers (e.g. std::set, std::map). The order reflects the
     *  order in which the underlying types were registered with WTF and does
     *  NOT reflect any numeric property (e.g. precision) of the types
     *  themselves.
     *
     *  @param[in] other The FloatKind to compare against *this.
     *
     *  @return True if *this was registered before @p other and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator<(const FloatKind& other) const noexcept {
        return m_id_ < other.m_id_;
    }

    /** @brief The id uniquely identifying the type held by *this.
     *
     *  @return The id uniquely identifying the type held by *this.
     *
     *  @throw None No throw guarantee.
     */
    id_type id() const noexcept { return m_id_; }

private:
    template<concepts::FloatingPoint T>
    friend FloatKind float_kind();

    friend FloatKind from_string(const std::string& name);

    /** @brief Creates a new FloatKind wrapping @p id.
     *
     *  This constructor is considered an implementation detail and is thus
     *  private. Users of the FloatKind class should use the float_kind or
     *  from_string functions to create instances of this class.
     *
     *  @param[in] id The id to wrap.
     */
    explicit FloatKind(id_type id) noexcept : m_id_(id) {}

    /// The id uniquely identifying the type held by *this
    id_type m_id_;
};

/** @brief Returns a human-readable name for the type identified by @p kind.
 *
 *  @related FloatKind
 *
 *  @param[in] kind The FloatKind to convert to a string.
 *
 *  @return The name of the type identified by @p kind, i.e. the value it was
 *          registered under.
 *
 *  @throw None No throw guarantee. FloatKind objects always identify a
 *              registered type, so the lookup underlying this function
 *              cannot fail.
 */
inline std::string to_string(FloatKind kind) {
    return detail_::FloatKindRegistry::instance().name_of(kind.id());
}

/** @brief Returns the FloatKind registered under @p name.
 *
 *  @related FloatKind
 *
 *  @param[in] name The name to look up, e.g. as returned by to_string().
 *
 *  @return The FloatKind registered under @p name.
 *
 *  @throw std::runtime_error if @p name is not registered with WTF. Strong
 *                            throw guarantee.
 */
inline FloatKind from_string(const std::string& name) {
    return FloatKind(detail_::FloatKindRegistry::instance().id_of(name));
}

/** @brief Returns the FloatKind identifying @p T.
 *
 *  @related FloatKind
 *
 *  @tparam T The floating-point type to get the FloatKind for. Must satisfy
 *            the concepts::FloatingPoint concept.
 *
 *  This function is the preferred way to create new instances of FloatKind
 *  for a compile-time type. The first time this function is called for a
 *  given T, T is registered with WTF's runtime enumerator infrastructure
 *  (see wtf::enums::detail_::FloatKindRegistry); subsequent calls simply
 *  return the previously assigned FloatKind. WTF_REGISTER_FP_TYPE(T) also
 *  calls this function once (as a static-initialization side effect), so in
 *  practice users of that macro never need to call this function themselves.
 *
 *  @return The FloatKind identifying @p T.
 *
 *  @throw std::bad_alloc if there is a problem registering @p T. Strong
 *                        throw guarantee.
 */
template<concepts::FloatingPoint T>
FloatKind float_kind() {
    using clean_t = std::remove_cv_t<T>;
    auto id       = detail_::FloatKindRegistry::instance().register_name(
      std::string(type_traits::type_name_v<clean_t>));
    return FloatKind(id);
}

} // namespace wtf::enums

/// Specializes std::hash so FloatKind objects can be used as keys in
/// unordered associative containers.
template<>
struct std::hash<wtf::enums::FloatKind> {
    std::size_t operator()(const wtf::enums::FloatKind& kind) const noexcept {
        return std::hash<wtf::enums::FloatKind::id_type>{}(kind.id());
    }
};
