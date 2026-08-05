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
#include <wtf/concepts/wtf_float.hpp>
#include <wtf/rtti/type_info.hpp>

namespace wtf::fp::detail_ {

/** @brief Defines the interface used to interact with aliased type-erased
 *         floats.
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::WTFFloat concept.
 *
 *  This class is the base class for all type-erased floating point holders
 *  that alias their state. The FloatView class interacts with this class and
 *  then this class redirects the implementation to the typed derived class.
 */
template<concepts::WTFFloat FloatType>
class FloatViewHolder {
public:
    /// Type of *this
    using holder_type = FloatViewHolder<FloatType>;

    /// Type of a holder aliasing a const FloatType
    using const_holder_type = FloatViewHolder<const FloatType>;

    /// Type of a pointer to a holder_type object
    using holder_pointer = std::unique_ptr<holder_type>;

    /// Type of a pointer to a const_holder_type object
    using const_holder_pointer = std::unique_ptr<const_holder_type>;

    /// Type of a read-only reference to a holder_type object
    using const_holder_reference = const holder_type&;

    /// Type of the RTTI container
    using type_info = rtti::TypeInfo;

    /// Read-only reference to type_info object
    using const_type_info_reference = const type_info&;

    /// Type of a string representation of *this
    using string_type = std::string;

    /// Default virtual destructor
    virtual ~FloatViewHolder() = default;

    /** @brief Makes a deep polymorphic copy of *this.
     *
     *  This method is used to make a copy of *this polymorphically. This
     *  means it will copy the state in any derived classes as well. This
     *  method is implemented by calling clone_(). View copies are such that
     *  the result still aliases the same value as *this, but can be made to
     *  alias a different value without modifying *this.
     *
     *  @return A unique_ptr to a deep copy of *this.
     */
    holder_pointer clone() const { return holder_pointer(clone_()); }

    /** @brief Makes a polymorphic copy of *this such that the copy is read-only
     *
     *  This method is used to make a copy of *this polymorphically, adding
     *  read-only character to the result. This means it will copy the state
     *  in any derived classes as well. This method is implemented by calling
     *  const_clone_().
     *
     *  @return A unique_ptr to a read-only copy of *this.
     *
     *  @throw std::bad_alloc if memory could not be allocated for the copy.
     *                        Strong throw guarantee.
     */
    const_holder_pointer const_clone() const {
        return const_holder_pointer(const_clone_());
    }

    /** @brief Wraps the process of changing the held value.
     *
     *  This method is used to change the value held by *this to that held by
     *  @p other. The types of the held values must match.
     *
     *  To avoid complicating the implementation we simply throw if we're
     *  trying to change a constant alias. The user-facing FloatView class will
     *  rely on SFINAE to prevent such calls from being made in the first place.
     *
     *  @param[in] other The value to change to.
     *
     *  @throw std::invalid_argument if the type of the held value does not
     *                               match that of @p other. Strong throw
     *                               guarantee.
     *  @throw std::runtime_error if *this is holding a constant value. Strong
     *                            throw guarantee.
     */
    void change_value(const_holder_reference other);

    /** @brief Provides access to the RTTI of the derived type.
     *
     *  @return A read-only reference to the RTTI object for the type held by
     *          the derived class.
     *
     *  @throw None No throw guarantee.
     */
    const_type_info_reference type() const { return m_type_; }

    /** @brief Does *this contain an object of type @p T?
     *
     *  @tparam T The type to check against. Must satisfy the
     *            concepts::FloatingPoint concept.
     *
     *  @return True if the type held by *this is exactly @p T, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<concepts::FloatingPoint T>
    bool is_type() const {
        return m_type_ == rtti::wtf_typeid<T>();
    }

    /** @brief Determines if the object held by *this can be converted to the
     *         type in @p other.
     *
     *   @tparam TupleType A std::tuple of types that are allowed to be in the
     *                     holders.
     *
     *   This method simply calls rtti::is_implicitly_convertible with the
     *   wrapped RTTI objects. See the documentation for that function for more
     *   details.
     *
     *   @param[in] other The other FloatViewHolder to check convertibility
     * against.
     *
     *   @return True if the type held by *this can be implicitly converted to
     *           the type held by @p other, false otherwise.
     *
     *   @throw None No throw guarantee.
     */
    template<typename TupleType>
    bool is_implicitly_convertible_to(const_holder_reference other) {
        return rtti::is_implicitly_convertible<TupleType>(type(), other.type());
    }

    /** @brief Polymorphic value comparison.
     *
     *  This method determines if the value held by *this is exactly equal
     *  to that held by @p other. The comparison is done polymorphically, i.e.,
     *  it will also ensure the state in the derived parts of *this and @p other
     *  are value equal.  This method is implemented by calling are_equal_.
     *
     *  @note The comparison will exactly compare the floating point values
     *        meaning they must be bitwise equal for built-in floating point
     *        types.
     *
     *  @param[in] other The other FloatViewHolder to compare against *this.
     *
     *  @return True if *this and @p other are polymorphically equal and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<concepts::WTFFloat OtherFloatType>
    bool are_equal(const FloatViewHolder<OtherFloatType>& other) const {
        if constexpr(std::is_same_v<FloatType, OtherFloatType>) {
            return type() == other.type() ? are_equal_(other) : false;
        } else {
            return false;
        }
    }

    /** @brief Returns a string-representation of the held value.
     *
     *  This method is used to get a string-representation of the floating-point
     *  object held by *this. This method is implemented by calling
     *  to_string_().
     *
     *  @return A string representing the held floating-point value.
     *
     *  @throw ??? Throws if converting the type-erased value to a string
     */
    string_type to_string() const { return to_string_(); }

protected:
    /// Initializes *this with the given type info object
    explicit FloatViewHolder(type_info ti) : m_type_(std::move(ti)) {}

private:
    /// Clones *this polymorphically
    virtual holder_type* clone_() const = 0;

    /// Clones *this polymorphically and adding read-only character to it
    virtual const_holder_type* const_clone_() const = 0;

    /// Base checked that types are equal, derived need only change values
    virtual void change_value_(const_holder_reference other) = 0;

    /// Base checks that types are equal, derived need only check values
    virtual bool are_equal_(const_holder_reference other) const = 0;

    /// Derived class should override to provide string representation
    virtual string_type to_string_() const = 0;

    /// The RTTI object for the object held by the derived class
    type_info m_type_;
};

// -----------------------------------------------------------------------------
// Out-of-line definitions
// -----------------------------------------------------------------------------

template<concepts::WTFFloat FloatType>
void FloatViewHolder<FloatType>::change_value(const FloatViewHolder& other) {
    if(type() == other.type()) {
        change_value_(other);
    } else {
        throw std::invalid_argument(
          "FloatHolder::set_value: Types do not match");
    }
}

} // namespace wtf::fp::detail_
