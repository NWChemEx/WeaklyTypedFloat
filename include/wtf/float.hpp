#pragma once
#include <memory>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/float_model.hpp>

namespace wtf {

/** @brief Type-erases a floating-point number.
 *
 *  This class type-erases a floating-point number. It can hold any type that
 *  satisfies the concepts::UnmodifiedFloatingPoint concept. The held type is
 *  mutable as long as *this is not const. This is because *this owns the value
 *  it stores and does NOT alias it.
 */
class Float {
public:
    /// Type defining the API for accessing the type-erased value
    using holder_type = detail_::FloatHolder;

    /// Type of a pointer to the holder_type
    using holder_pointer = holder_type::holder_pointer;

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

private:
    template<concepts::FloatingPoint T>
    friend Float make_float(T value);

    template<typename T>
        requires concepts::UnmodifiedFloatingPoint<std::decay_t<T>>
    friend T float_cast(Float& f);

    /// Creates a Float from an already existing holder. Used by make_float.
    explicit Float(holder_pointer holder) : m_holder_(std::move(holder)) {}

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
    return rhs == lhs;
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
    return !(rhs == lhs);
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
T float_cast(Float& f) {
    auto* p            = f.m_holder_.get();
    using derived_type = detail_::FloatModel<std::decay_t<T>>;
    auto pderived      = dynamic_cast<derived_type*>(p);
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::float_cast: bad cast");
    }
    return *pderived->data();
}
} // namespace wtf