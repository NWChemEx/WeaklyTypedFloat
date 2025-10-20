#pragma once
#include <utility> // for std::move, std::swap
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/float_view_holder.hpp>
#include <wtf/forward.hpp>
#include <wtf/type_traits/float_traits.hpp>

namespace wtf::detail_ {

/** @brief Implements the FloatViewHolder API for floats of type @p FloatType.
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::UnmodifiedFloatingPoint concept.
 *
 *  This class is responsible for aliasing and knowing the type of a single
 *  floating-point value. It implements the FloatViewHolder API defined by the
 *  base so that the held value can be manipulated in a type-erased manner.
 */
template<concepts::FloatingPoint FloatType>
class FloatViewModel
  : public FloatViewHolder<
      std::conditional_t<std::is_const_v<FloatType>, const Float, Float>> {
private:
    /// Is this a view of a const Float?
    static constexpr bool is_const_model_ = std::is_const_v<FloatType>;

    /// Accounting for const-ness what are we a view of?
    using wtf_float_type =
      std::conditional_t<is_const_model_, const Float, Float>;

public:
    /// Type *this derives from
    using holder_type = FloatViewHolder<wtf_float_type>;

    /// Pull in types from the base
    ///@{
    using typename holder_type::const_holder_reference;
    ///@}

    /// Type defining the traits for FloatType
    using float_traits = wtf::type_traits::float_traits<FloatType>;

    /// For convenience pull the types off of float_traits and make them part
    /// of this class's public interface.
    ///@{
    using value_type      = typename float_traits::value_type;
    using const_reference = typename float_traits::const_reference;
    using pointer         = typename float_traits::pointer;
    using const_pointer   = typename float_traits::const_pointer;
    ///@}

    /** @brief Makes @p value adhere to our Float API.
     *
     *  This constructor takes the address to a floating-point value and stores
     * it in *this. When accessed through the FloatViewModel API the value's
     * type is still known. Type-erasure occurs when FloatViewModel is accessed
     * through the base class, FloatViewHolder.
     *
     *  @param[in] value The floating-point value to be held.
     *
     *  @throw None No throw guarantee.
     */
    explicit FloatViewModel(pointer value) :
      holder_type(rtti::wtf_typeid<FloatType>()), m_pvalue_(std::move(value)) {}

    // *************************************************************************
    // Data Access and Modification
    // *************************************************************************

    /** @brief Provides read-only access to the held value.
     *
     *  This function provides read-only access to the held value.
     *
     *  @return A const reference to the held value.
     *
     *  @throw None No throw guarantee.
     */
    const_reference get_value() const { return *m_pvalue_; }

    /** @brief Allows the user to change the held value to @p value.
     *
     *  This function is used to override the value of *this with the provided
     *  value.
     *
     *  @param[in] value The new value to be held.
     *
     *  @throw None No throw guarantee.
     */
    void set_value(value_type value) {
        if constexpr(!is_const_model_) {
            *m_pvalue_ = std::move(value);
        } else {
            throw std::runtime_error(
              "FloatViewModel::set_value: Attempt to modify const value");
        }
    }
    /** @brief Provides mutable access directly to the held value.
     *
     *  Get/set are designed to be one-directional, i.e., you can't use
     *  get_value to also set the value. Sometimes we need to return the value
     *  in a modifiable way, e.g., when the user is unwrapping the wrapped
     *  value. This method can be used to get the address of the held value so
     *  that it can be manipulated directly.
     *
     *  @return A pointer to the held value.
     *
     *  @throw None No throw guarantee.
     */
    pointer data() { return m_pvalue_; }

    /** @brief Provides read-only access directly to the held value.
     *
     *  This is the const-qualified version of data(). See the documentation for
     *  the non-const version for more details.
     *
     *  @return A read-only pointer to the held value.
     *
     *  @throw None No throw guarantee.
     */
    const_pointer data() const { return m_pvalue_; }

    // *************************************************************************
    // Utility
    // *************************************************************************

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  This method swaps the state of *this with that of @p other.
     *
     *  @param[in,out] other The other FloatViewModel to swap with *this. After
     * the operation @p other will contain the state that *this had before the
     * call.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FloatViewModel& other) noexcept {
        using std::swap;
        swap(m_pvalue_, other.m_pvalue_);
    }

    /** @brief  Determines if *this is exactly equal to @p other.
     *
     *  Two FloatModels are exactly equal if the values they hold compare equal
     *  using operator==. Note that this is bitwise equality for built-in
     *  floating-point types (and should realistically be implemented similarly
     *  for user-defined types as well). This check will also require that the
     *  types of the objects being compared are the same.
     *
     *  @param[in] other The other FloatViewModel to compare against *this.
     *
     *  @return True if *this and @p other are exactly equal, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FloatViewModel& other) const {
        return get_value() == other.get_value();
    }

    /** @brief Determines if *this and @p other are different.
     *
     *  This method defines different as not exactly equal, i.e., this method
     *  simply negates the result of operator==. See operator== for more
     *  details.
     *
     *  @param[in] other The other FloatViewModel to compare against *this.
     *
     *  @return False if *this and @p other are equal and true otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool operator!=(const FloatViewModel& other) const {
        return !(*this == other);
    }

private:
    /// Implements clone() by making a new FloatViewModel with the copy
    holder_type* clone_() const override { return new FloatViewModel(*this); }

    /// Implements FloatHolder::change_value by downcasting and calling
    /// set_value
    void change_value_(const_holder_reference other) override {
        if constexpr(!is_const_model_) {
            if(auto* p = dynamic_cast<const FloatViewModel*>(&other)) {
                set_value(p->get_value());
            } else {
                throw std::invalid_argument(
                  "FloatViewModel::change_value_: Dynamic cast failed");
            }
        } else {
            throw std::runtime_error(
              "FloatViewModel::change_value_: Attempt to modify const value");
        }
    }

    /// Implements FloatHolder::are_equal by downcasting and calling operator==
    bool are_equal_(const_holder_reference other) const override {
        if(auto* p = dynamic_cast<const FloatViewModel*>(&other)) {
            return *this == *p;
        }
        return false;
    }

    /// The value being held
    pointer m_pvalue_;
};

} // namespace wtf::detail_