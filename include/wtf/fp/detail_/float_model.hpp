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
#include <utility> // for std::move, std::swap
#include <wtf/concepts/floating_point.hpp>
#include <wtf/fp/detail_/float_holder.hpp>
#include <wtf/fp/detail_/float_view_model.hpp>
#include <wtf/type_traits/float_traits.hpp>

namespace wtf::fp::detail_ {

/** @brief Implements the FloatHolder API for floats of type @p FloatType.
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::UnmodifiedFloatingPoint concept.
 *
 *  This class is responsible for holding and knowing the type of a single
 *  floating-point value. It implements the FloatHolder API defined by the
 *  base so that the held value can be manipulated in a type-erased manner.
 */
template<concepts::FloatingPoint FloatType>
class FloatModel : public FloatHolder {
public:
    /// Type *this derives from
    using holder_type = FloatHolder;

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
     *  This constructor takes a floating-point value and stores it in *this.
     *  When accessed through the FloatModel API the value's type is still
     *  known. Type-erasure occurs when FloatModel is accessed through the base
     *  class, FloatHolder.
     *
     *  @param[in] value The floating-point value to be held.
     *
     *  @throw None No throw guarantee.
     */
    explicit FloatModel(value_type value) :
      FloatHolder(rtti::wtf_typeid<FloatType>()), value_(std::move(value)) {}

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
    const_reference get_value() const { return value_; }

    /** @brief Allows the user to change the held value to @p value.
     *
     *  This function is used to override the value of *this with the provided
     *  value.
     *
     *  @param[in] value The new value to be held.
     *
     *  @throw None No throw guarantee.
     */
    void set_value(value_type value) { value_ = std::move(value); }

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
    pointer data() { return &value_; }

    /** @brief Provides read-only access directly to the held value.
     *
     *  This is the const-qualified version of data(). See the documentation for
     *  the non-const version for more details.
     *
     *  @return A read-only pointer to the held value.
     *
     *  @throw None No throw guarantee.
     */
    const_pointer data() const { return &value_; }

    // *************************************************************************
    // Utility
    // *************************************************************************

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  This method swaps the state of *this with that of @p other.
     *
     *  @param[in,out] other The other FloatModel to swap with *this. After the
     *                       operation @p other will contain the state that
     *                       *this had before the call.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FloatModel& other) noexcept {
        using std::swap;
        swap(value_, other.value_);
    }

    /** @brief  Determines if *this is exactly equal to @p other.
     *
     *  Two FloatModels are exactly equal if the values they hold compare equal
     *  using operator==. Note that this is bitwise equality for built-in
     *  floating-point types (and should realistically be implemented similarly
     *  for user-defined types as well). This check will also require that the
     *  types of the objects being compared are the same.
     *
     *  @param[in] other The other FloatModel to compare against *this.
     *
     *  @return True if *this and @p other are exactly equal, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FloatModel& other) const {
        return value_ == other.value_;
    }

    /** @brief Determines if *this and @p other are different.
     *
     *  This method defines different as not exactly equal, i.e., this method
     *  simply negates the result of operator==. See operator== for more
     *  details.
     *
     *  @param[in] other The other FloatModel to compare against *this.
     *
     *  @return False if *this and @p other are equal and true otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool operator!=(const FloatModel& other) const { return !(*this == other); }

private:
    /// Implements clone() by making a new FloatModel with the copy
    holder_type* clone_() const override { return new FloatModel(*this); }

    /// Implements as_view by making a FloatViewModel aliasing the held value
    float_view_type* as_view_() override {
        using view_type = FloatViewModel<FloatType>;
        return new view_type(data());
    }

    /// Implements as_view const by making a const FloatViewModel
    const_float_view_type* as_view_() const override {
        using const_view_type = FloatViewModel<const FloatType>;
        return new const_view_type(data());
    }

    /// Implements FloatHolder::change_value by downcasting and calling
    /// set_value
    void change_value_(const FloatHolder& other) override {
        if(auto* p = dynamic_cast<const FloatModel*>(&other)) {
            set_value(p->get_value());
        } else {
            throw std::invalid_argument(
              "FloatModel::change_value_: Dynamic cast failed");
        }
    }

    /// Implements FloatHolder::are_equal by downcasting and calling operator==
    bool are_equal_(const FloatHolder& other) const override {
        if(auto* p = dynamic_cast<const FloatModel*>(&other)) {
            return *this == *p;
        }
        return false;
    }

    /// The value being held
    value_type value_;
};

} // namespace wtf::fp::detail_
