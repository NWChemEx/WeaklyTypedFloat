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
#include <wtf/concepts/floating_point.hpp>
#include <wtf/fp/detail_/float_model.hpp>
#include <wtf/fp/float_base.hpp>
#include <wtf/fp/float_view.hpp>
#include <wtf/warnings.hpp>

namespace wtf::fp {

/** @brief Type-erases a floating-point number.
 *
 *  This class type-erases a floating-point number. It can hold any type that
 *  satisfies the concepts::UnmodifiedFloatingPoint concept. The held type is
 *  mutable as long as *this is not const. This is because *this owns the value
 *  it stores and does NOT alias it.
 */
class Float : public FloatBase<Float> {
private:
    /// The type *this derives from
    using base_type = FloatBase<Float>;

public:
    /// Type defining the API for accessing the type-erased value
    using holder_type = detail_::FloatHolder;

    /// Type of a pointer to the holder_type
    using holder_pointer = holder_type::holder_pointer;

    /// Type of a view acting like *this
    using view_type = FloatView<Float>;

    /// Type of a read-only view acting like *this
    using const_view_type = FloatView<const Float>;

    /// Type of converting *this to a string
    using string_type = holder_type::string_type;

    /// Pull in types from the base class
    ///@{
    using typename base_type::rtti_type;
    ///@}

    // -------------------------------------------------------------------------
    // Ctors and assignment operators
    // -------------------------------------------------------------------------

    /** @brief Creates a Float by wrapping a floating point value.
     *
     *  @tparam T The type of floating-point value being wrapped. Must satisfy
     *            the concepts::UnmodifiedFloatingPoint concept. T will be
     *            deduced by the compiler.
     *
     *  This ctor is used to wrap a floating-point value in a Float. It is not
     *  explicit in order to facilitate implicit conversions and allow type-
     *  erased and typed floating-point objects to interoperate more easily.
     *
     *  @param[in] value The floating-point value to be wrapped.
     *
     *  @throw std::bad_alloc if there is a problem creating the holder. Strong
     *                        throw guarantee.
     */
    template<concepts::UnmodifiedFloatingPoint T>
    Float(T value) :
      m_holder_(std::make_unique<detail_::FloatModel<T>>(std::move(value))) {}

    /** @brief Creates a new Float by deep copying @p other.
     *
     *  This is the copy ctor. It makes a deep copy of @p other.
     *
     *  @param[in] other The Float to make a deep copy of.
     *
     *  @throw std::bad_alloc if there is a problem making the deep copy. Strong
     *                        throw guarantee.
     */
    Float(const Float& other) : m_holder_(other.m_holder_->clone()) {}

    /** @brief Creates a new Float by taking the state from @p other.
     *
     *  This is the move ctor. It takes the state from @p other.
     *
     *  @param[in,out] other The Float to move from. After this operation
     *                       @p other will be in a valid but unspecified state.
     *
     *  @throw None No throw guarantee.
     */
    Float(Float&& other) noexcept = default;

    /** @brief Overrides the state of *this with a copy of @p other.
     *
     *  This is the copy assignment operator. It makes a deep copy of @p other
     *  and overrides the state of *this with that copy. Whatever state was in
     *  *this will be destroyed.
     *
     *  @param[in] other The Float to make a deep copy of.
     *
     *  @return A reference to *this.
     *
     *  @throw std::bad_alloc if there is a problem making the deep copy. Strong
     *                        throw guarantee.
     */
    Float& operator=(const Float& other);

    /** @brief Overrides the state in *this by taking the state of @p other.
     *
     *  This is the move assignment operator. It takes the state from @p other
     *  and overrides the state of *this with that state. Whatever state was in
     *  *this will be destroyed. After this operation @p other will be in a
     *  valid but unspecified state.
     *
     *  @param[in,out] other The Float to move from. After this operation
     *                       @p other will be in a valid but unspecified state.
     *
     *  @return A reference to *this.
     *
     *  @throw None No throw guarantee.
     */
    Float& operator=(Float&& other) noexcept = default;

    // -------------------------------------------------------------------------
    // Utility methods
    // -------------------------------------------------------------------------

    /** @brief Explicitly converts a Float object to a FloatView.
     *
     *  Float objects are implicitly convertible to FloatView objects. This
     *  way APIs written to accept FloatView objects will also accept Float
     *  objects. However, sometimes it is useful to be able to explicitly
     *  convert a Float to a FloatView. This method provides that functionality.
     *
     *  @return A FloatView that aliases the floating-point value held by *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the FloatView.
     *                        Strong throw guarantee.
     */
    auto as_view() { return view_type(m_holder_->as_view()); }

    /** @brief Explicitly converts a const Float object to a const FloatView.
     *
     *  This method is the const version of the non-const as_view() method.
     *  It provides the ability to explicitly convert a const Float to a
     *  FloatView<const Float>.
     *
     *  @return A FloatView<const Float> that aliases the floating-point value
     *          held by *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the FloatView.
     *                        Strong throw guarantee.
     */
    auto as_view() const {
        return const_view_type(std::as_const(*m_holder_).as_view());
    }

    /** @brief Exchanges the contents of *this with that of @p other.
     *
     *  This method is used to exchange the stat of *this with the state of
     *  @p other.
     *
     *  @param[in,out] other The other Float to swap with *this. After this
     *                       operation @p other will contain the state which
     *                       was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(Float& other) noexcept { m_holder_.swap(other.m_holder_); }

    /** @brief Determines if two Float objects are value equal.
     *
     *  Value equality for Float objects is a bit tricky. Two Float objects
     *  are value equal if the values they wrap have exactly the same type
     *  (e.g., a double and a float are not value equal even if they both were
     *  initialized to the same literal) and if the values compare equal using
     *  operator==. Note that this is bitwise equality for built-in floating-
     *  point types (and should realistically be implemented similarly for user-
     *  defined types as well).
     *
     *  @param[in] other The other Float to compare against *this.
     *
     *  @return True if *this and @p other are value equal, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Float& other) const {
        return m_holder_->are_equal(*other.m_holder_);
    }

    /** @brief Determines if two Float objects are different.
     *
     *  This method defines different as not value equal, i.e., this method
     *  simply negates the result of operator==. See operator== for more
     *  details.
     *
     *  @param[in] other The other Float to compare against *this.
     *
     *  @return False if *this and @p other are value equal and true otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool operator!=(const Float& other) const { return !(*this == other); }

    /** @brief Provides a string representation of the held value.
     *
     *  If *this is not holding a value, an empty string is returned.
     *
     *  @return A string representation of the held value.
     *
     *  @throw ??? Throws if converting the held value to a string throws.
     *             Strong throw guarantee.
     */
    string_type to_string() const {
        return is_holding_() ? m_holder_->to_string() : string_type{};
    }

private:
    /// Determines if *this is holding a value or not
    bool is_holding_() const noexcept { return m_holder_ != nullptr; }

    template<typename TupleType, typename Visitor, typename... Args>
    friend auto visit_float(Visitor&& visitor, Args&&... args);

    template<concepts::FloatingPoint T>
    friend Float make_float(T value);

    auto& holder_() { return *m_holder_; }
    const auto& holder_() const { return *m_holder_; }

    template<typename T>
        requires concepts::UnmodifiedFloatingPoint<std::decay_t<T>>
    friend T float_cast(Float& f);

    /// Creates a Float from an already existing holder. Used by make_float.
    explicit Float(holder_pointer holder) : m_holder_(std::move(holder)) {}

    /** @brief Returns the RTTI of the held float.
     *
     *  This method returns the RTTI information for the floating-point type.
     *  If *this does not hold a value, the RTTI information will describe a
     *  nullptr_t type.
     *
     *  @return The RTTI for the held floating-point type.
     *
     *  @throw std::bad_alloc if there is a problem creating the type info.
     *                        Strong throw guarantee.
     */
    rtti_type type_info_() const override {
        return is_holding_() ? m_holder_->type() :
                               rtti::wtf_typeid<std::nullptr_t>();
    }

    /// The holder that implements the type-erased floating-point API
    holder_pointer m_holder_;
};

/** @brief Allows floating-point objects to be compared to Floats.
 *
 *  @related Float
 *
 *  @tparam T The type of floating-point value being compared. Must satisfy
 *            the concepts::UnmodifiedFloatingPoint concept.
 *
 *  This operator allows floating-point values to be compared to Float. It is
 *  implemented in terms of the Float::operator== method and uses the same
 *  definition of value equality. See the documentation for that method for
 *  more details.
 *
 *  @param[in] lhs The floating-point value being compared.
 *  @param[in] rhs The Float being compared.
 *
 *  @return True if @p lhs and @p rhs are value equal, false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<concepts::UnmodifiedFloatingPoint T>
bool operator==(T lhs, const Float& rhs) {
    return Float(lhs) == rhs;
}

/** @brief Allows floating-point objects to be compared to Floats.
 *
 *  @related Float
 *
 *  @tparam T The type of floating-point value being compared. Must satisfy
 *            the concepts::UnmodifiedFloatingPoint concept.
 *
 *  This operator allows floating-point values to be compared to Float. It is
 *  implemented by negating the result of the Float::operator== method. See
 *  the documentation for that method for more details.
 *
 *  @param[in] lhs The floating-point value being compared.
 *  @param[in] rhs The Float being compared.
 *
 *  @return False if @p lhs and @p rhs are value equal, true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<concepts::UnmodifiedFloatingPoint T>
bool operator!=(T lhs, const Float& rhs) {
    return !(lhs == rhs);
}

/** @brief Helps create a Float object from an object of type @p T.
 *
 *  @related Float
 *
 *  @tparam T The type of floating-point value being wrapped. Must satisfy
 *            the concepts::FloatingPoint concept. T will be deduced by the
 *            compiler, but may be optionally specified.
 *
 *  This function is a convenience function to help create a Float object from
 *  a floating-point value. Its interface is defined to be similar to that of
 *  std::make_any. While one can create a Float object directly from the ctor,
 *  the ctor will always wrap the provided argument as the argument's type.
 *  While this may sound obvious, in practice it can be tricky to know the
 *  type depending on how the floating-point value was declared. Since one can
 *  not specify the template argument to the Float ctor directly, there is not
 *  an easy mechanism for the user to ensure the wrapped type is what they
 *  want.
 *
 *  On the contrary, this function allows the user to specify the type while
 *  wrapping. For example:
 *
 *  ```cpp
 *  auto val = 3.14; // val is a double
 *  Float f1(val); // wraps val as a double
 *  Float f2(3.14); // Also would wrap 3.14 as a double
 *  Float f3 = make_float<float>(val); // wraps val as a float
 *  ```
 *
 *  @param[in] value The floating-point value to be wrapped.
 *
 *  @return A Float object wrapping @p value.
 *
 *  @throw std::bad_alloc if there is a problem creating the holder. Strong
 *                        throw guarantee.
 */
template<concepts::FloatingPoint T>
Float make_float(T value) {
    return Float(std::make_unique<detail_::FloatModel<T>>(std::move(value)));
}

/** @brief Used to get the typed float back.
 *
 *  @related Float
 *
 *  @tparam T The type of floating-point value being extracted. std::decay_t<T>
 *            must satisfy the concepts::UnmodifiedFloatingPoint concept.
 *
 *  This function is used to extract the typed floating-point value from a
 *  the type-erased Float object.
 *
 *  @param[in] f The Float object to extract the value from.
 *
 *  @return The floating-point value held by @p f.
 */
template<typename T>
    requires concepts::UnmodifiedFloatingPoint<std::decay_t<T>>
IGNORE_DANGLING_REFERENCE T float_cast(Float& f) {
    auto* p            = f.m_holder_.get();
    using derived_type = detail_::FloatModel<std::decay_t<T>>;
    auto pderived      = dynamic_cast<derived_type*>(p);
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::float_cast: bad cast");
    }
    return *pderived->data();
}

/** @brief Wraps the process of visiting zero or more Float objects.
 *
 *  @relates Float
 *
 *  @tparam TupleType A std::tuple of floating-point types to try. Must be
 *                    provided by the caller.
 *  @tparam Visitor The type of the visitor to call. Must be a callable object.
 *                 Will be inferred by the compiler.
 *  @tparam Args The cv-qualified Float objects to type-restore. Will be
 *               inferred by the compiler.
 *
 *  The visitor should have the signature: `R(T, U, ...)` where T, U, ... are
 *  types found in @p TupleType and R is the return type (which may be void).
 *  T, U, ... may be cv-qualified and may repeat types.
 *
 *  @return The result of invoking @p visitor with the type-restored Float
 *          objects.
 */
template<typename TupleType, typename Visitor, typename... Args>
auto visit_float(Visitor&& visitor, Args&&... args) {
    return visit_float_model<TupleType>(std::forward<Visitor>(visitor),
                                        (args.holder_())...);
};

} // namespace wtf::fp
