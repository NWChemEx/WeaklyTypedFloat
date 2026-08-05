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
#include <memory>
#include <wtf/rtti/detail_/type_model.hpp>
#include <wtf/type_traits/type_name.hpp>

namespace wtf::rtti {

/** @brief Custom RTTI class for floating-point types.
 *
 * C++'s RTTI class is a bit lacking. Ours is better :wink:. In particular, ours
 * standardizes the state so that we know we get say the same name for the
 * same floating-point type across different compilers. It also provides
 * support for distinguishing between const and non-const types. Finally, it
 * provides some introspection properties, such as the precision of the type
 * and whether or not the type can be implicitly converted to another type.
 *
 * @note To create instances of this class, use the wtf_typeid function.
 */
class TypeInfo {
public:
    /// Type of the type-erased holder that can access the type
    using holder_type = detail_::TypeHolder;

    /// Type of a reference to an object of type holder_type
    using holder_reference = holder_type&;

    /// Type of a read-only reference to an object of type holder_type
    using const_holder_reference = const holder_type&;

    /// Pull types from holder_type to make them part of this class's API
    ///@{
    using holder_pointer         = holder_type::holder_pointer;
    using const_string_reference = holder_type::const_string_reference;
    ///@}

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------

    /** @brief The name of the type held by *this
     *
     *  This method is used to get a human-readable name for the type held by
     *  *this. The names are standardized by WTF so that they will be the same
     *  across compilers (unlike typeid(T).name(), which is implementation
     *  defined). The names will also include const qualifiers.
     *
     *  @return The name of the type held by *this
     *
     *  @throw None No throw guarantee.
     */
    const_string_reference name() const noexcept;

    // -------------------------------------------------------------------------
    // Utility functions
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p other.
     *
     *  Two TypeInfo objects are considered value equal if they type-erase the
     *  same type. Const and non-const types are considered in the comparison.
     *
     *  @param[in] other The TypeInfo to compare against *this.
     *
     *  @return True if *this and @p other type-erase the same type and false
     *               otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const TypeInfo& other) const;

    /** @brief Determines if *this and @p other are different.
     *
     *  Two TypeInfo objects are considered different if they are not value
     *  equal. This method simply negates the result of operator==. See the
     *  documentation for operator== for details on how value equality is
     *  determined.
     *
     *  @param[in] other The TypeInfo to compare against *this.
     *
     *  @return True if *this and @p other are not value equal and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const TypeInfo& other) const;

private:
    template<typename T>
    friend TypeInfo wtf_typeid();

    template<typename TupleType>
    friend bool is_implicitly_convertible(const TypeInfo& ti1,
                                          const TypeInfo& ti2);

    /** @brief  Creates a new TypeInfo by wrapping a pointer to a type holder.
     *
     *  This constructor is considered an implementation detail and is thus
     *  private. Users of the TypeInfo class should use the wtf_typeid function
     *  to create instances of this class.
     *
     *  @param[in] holder The type holder to wrap.
     */
    explicit TypeInfo(holder_pointer holder);

    /** @brief Can the type held by *this be implicitly converted to that held
     *         by @p other?
     *
     *  @tparam TypeTuple A std::tuple of types. @p TypeTuple must contain the
     *                    type that both *this and @p other hold. If it doesn't
     *                    an exception will be raised.
     *
     *  This method implements the free function is_implicitly_convertible. We
     *  keep it private to avoid users having to deal with needing to specify
     *  the "template" keyword when calling it.
     *
     *  @param[in] other The type info for the type we want to convert to.
     */
    template<typename TypeTuple>
    bool is_implicitly_convertible_to(const TypeInfo& other) const;

    /// Wraps the process of dereferencing m_holder_
    holder_reference holder_();

    /// Wraps the process of dereferencing m_holder_
    const_holder_reference holder_() const;

    /// Pointer to the type-erased holder that can access the type
    holder_pointer m_holder_;
};

/** @brief Can the type held by @p ti1 be implicitly converted to the type held
 *         by @p ti2?
 *
 *  @related TypeInfo
 *
 *  @tparam TupleType A std::tuple of types. @p TupleType must contain the type
 *                    that both @p ti1 and @p ti2 hold. If it doesn't an
 *                    exception will be raised. In practice, users should
 *                    provide a std::tuple of all types they want to support.
 *
 *  This function essentially checks if a function like:
 *
 *   ```cpp
 *   ti2 convert(ti1) { return std::declval<ti1>(); }
 *   ```
 *
 *   would compile. In this function `ti1` and `ti2` are the types held by
 *   @p ti1 and @p ti2 respectively.
 *
 *   @param[in] ti1 The type info for the type we want to convert from.
 *   @param[in] ti2 The type info for the type we want to convert to.
 *
 *   @return True if the type held by @p ti1 can be implicitly converted to
 *           the type held by @p ti2. False otherwise.
 *
 *   @throw std::runtime_error if @p TupleType does not contain the type held
 *                             by either @p ti1 or @p ti2.
 */
template<typename TupleType>
bool is_implicitly_convertible(const TypeInfo& ti1, const TypeInfo& ti2) {
    return ti1.template is_implicitly_convertible_to<TupleType>(ti2);
}

/** @brief Creates a TypeInfo object for type @p T.
 *
 * @related TypeInfo
 * @tparam T The type for which to create a TypeInfo object. Must satisfy the
 *           concepts::FloatingPoint concept.
 *
 * This function is the preferred way to create new instances of TypeInfo
 * objects.
 */
template<typename T>
TypeInfo wtf_typeid() {
    auto name = type_traits::type_name_v<std::decay_t<T>>;
    detail_::TypeModel<T> model(name);
    return TypeInfo(model.get_model());
}

// -----------------------------------------------------------------------------
// Out of line definitions
// -----------------------------------------------------------------------------

template<typename TypeTuple>
bool TypeInfo::is_implicitly_convertible_to(const TypeInfo& other) const {
    const auto& holder2 = other.holder_();
    return holder_().is_implicitly_convertible_to<TypeTuple>(holder2);
}

} // namespace wtf::rtti
