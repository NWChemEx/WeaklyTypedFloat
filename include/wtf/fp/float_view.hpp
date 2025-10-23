#pragma once
#include <wtf/concepts/floating_point.hpp>
#include <wtf/concepts/wtf_float.hpp>
#include <wtf/fp/detail_/float_view_model.hpp>
#include <wtf/fp/float.hpp>

namespace wtf::fp {

/** @brief Aliases an existing floating-point value.
 *
 *  @tparam FloatType The type of wtf::Float this acts like. Must satisfy the
 *                    concepts::WTFFloat concept.
 *
 *  Float objects are objects which own their floating-point data and allow
 *  the user to interact with it in a type-erased manner. By contrast, FloatView
 *  objects only alias their floating-point data.
 *
 *  While the need to alias a floating-point value vs. copying it may at first
 *  seem unnecessary, there is a very important application of FloatView
 *  objects: providing type-erased access to buffers of floating-point values.
 *  In other words, we can use FloatView objects like type-erased pointers!
 */
template<concepts::WTFFloat FloatType>
class FloatView {
private:
    /// What is the type of *this
    using view_type = FloatView<FloatType>;

    /// What is the type of a FloatView aliasing a const FloatType
    using const_view_type = FloatView<const FloatType>;

    /// Is this a view of `const Float` object?
    static constexpr bool is_const = std::is_const_v<FloatType>;

    /// Type aliasing @p T (possibly mutable)
    template<concepts::FloatingPoint T>
    using mutable_model = detail_::FloatViewModel<T>;

    /// Type aliasing a read-only model of @p T
    template<concepts::FloatingPoint T>
    using const_model = detail_::FloatViewModel<const T>;

    /// Helps work out the const-correct model type for @p T
    template<concepts::FloatingPoint T>
    using model_type =
      std::conditional_t<is_const, const_model<T>, mutable_model<T>>;

public:
    /// The type of wtf::Float object *this is aliasing
    using float_type = FloatType;

    /// The type of the type-erased holder
    using holder_type = detail_::FloatViewHolder<FloatType>;

    /// Add types from holder_type to API
    ///@{
    using holder_pointer = typename holder_type::holder_pointer;
    ///@}

    /** @brief Creates a FloatView that aliases @p value.
     *
     *  @tparam T The type of floating-point value being aliased. Must satisfy
     *            the concepts::FloatingPoint concept.
     *
     *  @note This constructor can NOT be used like `FloatView fview(3.14)`
     *        because @p value must be an lvalue so that we can take its
     *        address. You will get a compiler error if you try to do this or
     *        something similar.
     *
     *  This constructor will create a new FloatView object that aliases
     *  @p value. If the value of @p value is changed through other means (e.g.,
     *  directly modifying the variable that was passed in) then the change will
     *  be reflected when accessing the value through *this. Similarly, changing
     *  the value through *this will change the original variable.
     *
     *  @param[in] value The floating-point value to be aliased.
     *
     *  @throw std::bad_alloc if memory for the internal holder can not be
     *                        allocated. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    FloatView(T& value) : m_pfloat_(std::make_unique<model_type<T>>(&value)) {}

    /** @brief Implicit conversion to a read-only alias.
     *
     *  @tparam FloatType2 The type of Float being aliased by @p other. This
     *                     ctor wil only participate in overload resolution if
     *                     FloatType is const-qualified and FloatType2 is not.
     *
     *  This ctor can be used to convert FloatView<Float> objects into
     *  FloatView<const Float> objects. This in turn allows us to pass
     *  FloatView<Float> objects to functions that expect
     *  FloatView<const Float> objects, without having to explicitly convert
     *  them.
     *
     *  @param[in] other The other FloatView to convert. The constructed
     *                   FloatView will alias the same floating-point value as
     *                   @p other.
     *
     * @throw std::bad_alloc if memory for the internal holder can not be
     *                       allocated. Strong throw guarantee.
     */
    template<concepts::WTFFloat FloatType2>
        requires(std::is_const_v<FloatType> && !std::is_const_v<FloatType2>)
    FloatView(const FloatView<FloatType2>& other) :
      m_pfloat_(other.m_pfloat_->const_clone()) {}

    /** @brief Creates a new FloatView by copying @p other.
     *
     *  A FloatView created by copying @p other will alias the same floating-
     *  point value as @p other. Thus the copy is NOT a deep copy.
     *
     *  @param[in] other The other FloatView to copy.
     *
     *  @throw std::bad_alloc if memory for the internal holder can not be
     *                        allocated. Strong throw guarantee.
     */
    FloatView(const FloatView& other) : m_pfloat_(other.m_pfloat_->clone()) {}

    /** @brief Move-constructs a new FloatView by stealing @p other 's state.
     *
     *  After the move, @p other will be in a valid but unspecified state.
     *
     *  @param[in,out] other The other FloatView to move from. After the move,
     *                       @p other will be in a valid but unspecified state.
     *
     *  @throw None No throw guarantee.
     */
    FloatView(FloatView&& other) noexcept = default;

    /** @brief Overrides the state of *this with a copy of @p other.
     *
     *  This method will change the FP value aliased by *this to be the same
     *  instance as that aliased by @p other. Thus this is NOT a deep copy.
     *
     *  @param[in] other The other FloatView to copy from.
     *
     *  @return A reference to *this.
     *
     *  @throw std::bad_alloc if memory for the internal holder can not be
     *                        allocated. Strong throw guarantee.
     */
    FloatView& operator=(const FloatView& other);

    /** @brief Overrides the state of *this by stealing the state of @p other.
     *
     *  After the move, @p other will be in a valid but unspecified state.
     *
     *  @param[in,out] other The other FloatView to move from. After the move,
     *                       @p other will be in a valid but unspecified state.
     *
     *  @return A reference to *this.
     *
     *  @throw None No throw guarantee.
     */
    FloatView& operator=(FloatView&& other) noexcept = default;

    /** @brief Sets the aliased value to @p other.
     *
     *  This method does NOT alias other. It actually sets the aliased value
     *  to @p other.
     *
     *  @param[in] other The new value to set the aliased value to.
     *
     *  @return A reference to *this.
     *
     *  @throw std::runtime_error if the type of @p other does not match. Strong
     *                            throw guarantee.
     */
    template<concepts::FloatingPoint T>
    FloatView& operator=(T other);

    // -------------------------------------------------------------------------
    // Utility methods
    // -------------------------------------------------------------------------

    /** @brief Causes *this to alias the value previously aliased by @p other.
     *
     *  This method swaps the state of *this with that of @p other. After the
     *  operation *this will alias the value that @p other did before the
     *  operation and @p other will alias the value that *this did before the
     *  operation.
     *
     *  @param[in,out] other The other FloatView to swap with *this. After the
     *                       operation @p other will alias the value that *this
     *                       had aliased before the call.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FloatView& other) noexcept { m_pfloat_.swap(other.m_pfloat_); }

    /** @brief Determines if two FloatViews alias value equal values.
     *
     *  @tparam OtherFloatType The type of Float being aliased by @p other.
     *                         Must satisfy the concepts::WTFFloat concept.
     *
     *  This method does NOT check if *this and @p other alias the same
     *  floating-point value. Instead it checks if the values they alias compare
     *  value equal using operator==. The definition of value equality depends
     *  on the types being compared, but in general will be bitwise equality for
     *  built-in floating-point types (and should realistically be implemented
     *  similarly for user-defined types as well).
     *
     *  @note If *this and @p other alias different types, the comparison will
     *        be false.
     *
     *  @param[in] other The other FloatView to compare against *this.
     *
     *  @return True if the values aliased by *this and @p other compare equal,
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<concepts::WTFFloat OtherFloatType>
    bool operator==(const FloatView<OtherFloatType>& other) const;

    /** @brief Determines if the aliased value is value equal @p other.
     *
     *  @tparam T The type of floating-point value being compared against. Must
     *            satisfy the concepts::FloatingPoint concept.
     *
     *  This method will compare the aliased value to @p other using operator==.
     *  The definition of value equality depends on the types being compared,
     *  but in general will be bitwise equality for built-in floating-point
     *  types (and should realistically be implemented similarly for user-
     *  defined types as well).
     *
     *  @param[in] other The floating-point value to compare against the aliased
     *                   value.
     *
     *  @return True if the aliased value and @p other compare equal, false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<concepts::FloatingPoint T>
    bool operator==(const T& other) const;

    /** @brief Determines if the aliased values are different.
     *
     *  @tparam OtherFloatType The type of Float being aliased by @p other.
     *                         Must satisfy the concepts::WTFFloat concept.
     *
     *  This method defines different as not value equal, i.e., this method
     *  simply negates the result of operator==. See operator== for more
     *  details.
     *
     *  @param[in] other The other FloatView to compare against *this.
     *
     *  @return False if the values aliased by *this and @p other compare equal
     *          and true otherwise.
     *
     *  @throw None no throw guarantee.
     */
    template<concepts::WTFFloat OtherFloatType>
    bool operator!=(const FloatView<OtherFloatType>& other) const {
        return !(*this == other);
    }

    /** @brief Un-type-erases the held value and returns it.
     *
     *  @tparam T The type of floating-point value to return. Up to type
     *            qualifiers must satisfy the concepts::FloatingPoint concept.
     *
     *  This method is used to un-type-erase the held floating-point value. Its
     *  use is a bit clunky, but it is necessary to invoke it with the
     *  "template" keyword. It is instead recommended that users use the
     *  float_cast<T>() free function instead.
     *
     *  @return The held floating-point value as type T.
     *
     *  @throw std::runtime_error if the held value can not be cast to T. Strong
     *                            throw guarantee.
     */
    template<typename T>
        requires concepts::FloatingPoint<std::decay_t<T>>
    T value();

    /** @brief Un-type-erases the held value and returns it.
     *
     *  @tparam T The type of floating-point value to return. Must satisfy the
     *            concepts::FloatingPoint concept and be either const-qualified
     *            or unmodified.
     *
     *  This method is used to un-type-erase the held floating-point value. Its
     *  use is a bit clunky, but it is necessary to invoke it with the
     *  "template" keyword. It is instead recommended that users use the
     *  float_cast<T>() free function instead.
     *
     *  @return The held floating-point value as type T.
     *
     *  @throw std::runtime_error if the held value can not be cast to T. Strong
     *                            throw guarantee.
     */
    template<typename T>
        requires(concepts::FloatingPoint<std::decay_t<T>> &&
                 (concepts::ConstQualified<T> || concepts::Unmodified<T>))
    T value() const;

private:
    template<concepts::WTFFloat FloatType2>
    friend class FloatView;

    template<concepts::FloatingPoint T>
    friend auto make_float_view(T& value);

    /// Common ctor used once the holder is created
    FloatView(holder_pointer pfloat) : m_pfloat_(std::move(pfloat)) {}

    /// The holder object
    holder_pointer m_pfloat_;
};

/** @brief Helper function for unwrapping a FloatView object.
 *
 *  @related FloatView
 *
 *  @tparam T The type of floating-point value to return. Up to type qualifiers
 *            must satisfy the concepts::FloatingPoint concept. The user must
 *            provide this template type parameter explicitly.
 *  @tparam FloatType The type of wtf::Float being aliased by @p fview. The
 *            compiler will deduce this type parameter.
 *
 *  This function calls FloatView<FloatType>::value<T>() internally, but affords
 *  the user a cleaner syntax for unwrapping the held floating-point value.
 *
 *  @param[in] fview The FloatView to unwrap. Taken by copy to allow passing
 *                   in rvalues.
 *
 *  @return The held floating-point value as type T.
 *
 *  @throw std::runtime_error if the held value can not be cast to T. Strong
 */
template<typename T, concepts::WTFFloat FloatType>
    requires concepts::FloatingPoint<std::decay_t<T>>
T float_cast(FloatView<FloatType> fview) {
    return fview.template value<T>();
}

/** @brief Helper function for creating a FloatView.
 *
 *  @related FloatView
 *
 *  @tparam T The type of floating-point value being aliased. Must satisfy
 *            the concepts::FloatingPoint concept.
 *
 *  This function creates a FloatView object that aliases @p value. It is a
 *  thin wrapper around the constructor of FloatView. The main advantage of
 *  using this function is that the compiler can deduce the template type
 *  parameter, making the syntax cleaner.
 *
 *  @param[in] value The floating-point value to be aliased.
 *
 *  @return A FloatView object that aliases @p value.
 *
 *  @throw std::bad_alloc if memory for the internal holder can not be
 *                        allocated. Strong throw guarantee.
 */
template<concepts::FloatingPoint T>
auto make_float_view(T& value) {
    auto pmodel = std::make_unique<detail_::FloatViewModel<T>>(&value);
    constexpr bool is_const = std::is_const_v<T>;
    using float_type        = std::conditional_t<is_const, const Float, Float>;
    return FloatView<float_type>(value);
}

// -----------------------------------------------------------------------------
// Inline implementations
// -----------------------------------------------------------------------------

template<concepts::WTFFloat FloatType>
FloatView<FloatType>& FloatView<FloatType>::operator=(const FloatView& other) {
    if(this != &other) {
        FloatView temp(other);
        swap(temp);
    }
    return *this;
}

template<concepts::WTFFloat FloatType>
template<concepts::FloatingPoint T>
FloatView<FloatType>& FloatView<FloatType>::operator=(T other) {
    using clean_t      = std::decay_t<T>;
    using derived_type = detail_::FloatViewModel<clean_t>;
    auto pderived      = dynamic_cast<derived_type*>(m_pfloat_.get());
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::FloatView::operator=: bad cast");
    }
    *(pderived->data()) = other;
    return *this;
}

template<concepts::WTFFloat FloatType>
template<concepts::WTFFloat OtherFloatType>
bool FloatView<FloatType>::operator==(
  const FloatView<OtherFloatType>& other) const {
    if constexpr(!std::is_same_v<FloatType, OtherFloatType>) {
        return const_view_type(*this) == const_view_type(other);
    } else {
        return m_pfloat_->are_equal(*other.m_pfloat_);
    }
}

template<concepts::WTFFloat FloatType>
template<concepts::FloatingPoint T>
bool FloatView<FloatType>::operator==(const T& other) const {
    if(m_pfloat_->template is_type<T>() ||
       m_pfloat_->template is_type<const T>()) {
        return float_cast<const T&>(*this) == other;
    }
    return false;
}

template<concepts::WTFFloat FloatType>
template<typename T>
    requires concepts::FloatingPoint<std::decay_t<T>>
T FloatView<FloatType>::value() {
    using clean_t      = std::decay_t<T>;
    using fp_type      = std::conditional_t<is_const, const clean_t, clean_t>;
    using derived_type = detail_::FloatViewModel<fp_type>;
    auto pderived      = dynamic_cast<derived_type*>(m_pfloat_.get());
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::FloatView::value: bad cast");
    }
    return *pderived->data();
}

template<concepts::WTFFloat FloatType>
template<typename T>
    requires(concepts::FloatingPoint<std::decay_t<T>> &&
             (concepts::ConstQualified<T> || concepts::Unmodified<T>))
T FloatView<FloatType>::value() const {
    using clean_t      = std::decay_t<T>;
    using fp_type      = std::conditional_t<is_const, const clean_t, clean_t>;
    using derived_type = detail_::FloatViewModel<fp_type>;
    auto pderived      = dynamic_cast<derived_type*>(m_pfloat_.get());
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::FloatView::value: bad cast");
    }
    return *pderived->data();
}

} // namespace wtf::fp