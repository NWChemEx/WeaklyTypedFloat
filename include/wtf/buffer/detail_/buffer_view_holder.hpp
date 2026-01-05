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
#include <wtf/concepts/float_buffer.hpp>
#include <wtf/fp/float_view.hpp>
#include <wtf/rtti/type_info.hpp>

namespace wtf::buffer::detail_ {

/** @brief Defines the interface between the type-erased and typed buffer.
 *
 *  This class provides BufferView with a type-erased interface for
 *  interacting with the held buffer. Derived classes implement the actual
 *  typed implementations.
 */
template<concepts::WTFFloat FloatType>
class BufferViewHolder {
public:
    /// Type of *this
    using holder_type = BufferViewHolder<FloatType>;

    /// Type of a holder type-erasing a read-only buffer
    using const_holder_type = BufferViewHolder<const FloatType>;

    /// Type of a pointer to *this
    using holder_pointer = std::unique_ptr<holder_type>;

    /// Type of a pointer to a const_holder_type
    using const_holder_pointer = std::unique_ptr<const_holder_type>;

    /// Type of a view to an element of *this
    using view_type = fp::FloatView<FloatType>;

    /// Type of a const view to an element of *this
    using const_view_type = fp::FloatView<const FloatType>;

    /// Type used to describe the RTTI of the held buffer
    using rtti_type = rtti::TypeInfo;

    /// Read-only reference to the RTTI of the held buffer
    using const_rtti_reference = const rtti_type&;

    /// Type used for indexing
    using size_type = std::size_t;

    /// Default virtual destructor
    virtual ~BufferViewHolder() = default;

    /** @brief Makes a shallow polymorphic copy of *this.
     *
     *  This method is used to make shallow copy of *this polymorphically. This
     *  means it will copy the state in any derived classes as well. This
     *  method is implemented by calling clone_(). The copy is shallow because
     *  the resulting holder will still alias the same underlying buffer as
     *  *this.
     *
     *  @return A unique_ptr to a deep copy of *this.
     *
     *  @throw std::bad_alloc if making the copy fails. Strong throw guarantee.
     */
    holder_pointer clone() const { return holder_pointer(clone_()); }

    /** @brief Makes a shallow copy that can only read the buffer.
     *
     *  This method is the same as clone except that the resulting holder can
     *  only read from the held buffer.
     *
     *  @return A unique_ptr to a const copy of *this.
     *
     *  @throw std::bad_alloc if making the copy fails. Strong throw guarantee.
     */
    const_holder_pointer const_clone() const {
        return const_holder_pointer(const_clone_());
    }

    /** @brief Retrieves the element with offset @p index.
     *
     *  This method is used to access an element of the held buffer in a type-
     *  erased manner.
     *
     *  @param[in] index The index of the element to retrieve.
     *
     *  @return A view to the element at index @p index.
     *
     *  @throw std::out_of_range if @p index is out of range. Strong throw
     *                           guarantee.
     */
    view_type at(size_type index) {
        assert_in_range(index);
        return at_(index);
    }

    /** @brief Retrieves the element with offset @p index.
     *
     *  This method is the same as the non-const version, except that the
     *  returned view is read-only.
     *
     *  @param[in] index The index of the element to retrieve.
     *
     *  @return A const view to the element at index @p index.
     *
     *  @throw std::out_of_range if @p index is out of range. Strong throw
     *                           guarantee.
     */
    const_view_type at(size_type index) const {
        assert_in_range(index);
        return at_(index);
    }

    /** @brief Returns the number of elements in the buffer.
     *
     *  @return The number of elements in the buffer.
     *
     *  @throw None No-throw guarantee.
     */
    size_type size() const noexcept { return size_(); }

    /** @brief What is the RTTI for the elements in the buffer?
     *
     *  This method returns the RTTI information for the elements in the buffer.
     *  It does NOT return the RTTI of the buffer.
     *
     *  @return A reference to the RTTI of the elements in the held buffer.
     *
     *  @throw None No-throw guarantee.
     */
    const_rtti_reference type() const { return m_rtti_; }

    /** @brief Are the held elements read-only?
     *
     *  @return true if the held elements are read-only, false otherwise.
     *
     *  @throw None No-throw guarantee.
     */
    bool is_const() const noexcept { return is_const_(); }

    /** @brief Are the elements in the buffer contiguous?
     *
     *  This method indicates whether the held buffer stores its elements
     *  contiguously in memory. If true, then it is safe to do pointer
     *  arithmetic off of the addresses of the typed elements. If false, then
     *  attempting to do arithmetic on the addresses will likely lead to
     *  undefined behavior.
     *
     *  @return true if the elements are contiguous, false otherwise.
     *
     *  @throw None No-throw guarantee.
     */
    bool is_contiguous() const { return is_contiguous_(); }

    /** @brief Is this polymorphically equal to @p other?
     *
     *  Two BufferHolders are considered equal if the elements they hold have
     *  the same type, use the same model, and the actual elements compare
     *  value equal. In general this requires the elements to be bitwise equal.
     *
     *  @param[in] other The other BufferViewHolder to compare against.
     *
     *  @return true if *this and @p other are equal, false otherwise.
     *
     *  @throw None No-throw guarantee.
     */
    bool are_equal(const holder_type& other) const {
        return m_rtti_ == other.m_rtti_ ? are_equal_(other) : false;
    }

protected:
    /// Used by derived class to initialize *this
    explicit BufferViewHolder(rtti_type rtti) : m_rtti_(std::move(rtti)) {}

    /// Better error message for out_of_range
    void assert_in_range(size_type index) const;

private:
    /// Clones *this polymorphically
    virtual holder_type* clone_() const = 0;

    /// Clones *this polymorphically as a const holder
    virtual const_holder_type* const_clone_() const = 0;

    /// Base ensures in bounds, derived should just return the element
    virtual view_type at_(size_type index) = 0;

    /// Base ensures in bounds, derived should just return the element
    virtual const_view_type at_(size_type index) const = 0;

    /// The size of the held buffer
    virtual size_type size_() const noexcept = 0;

    /// Does the derived class hold read-only data
    virtual bool is_const_() const noexcept = 0;

    /// Does the derived class store the buffer contiguously in memory?
    virtual bool is_contiguous_() const = 0;

    /// RTTI is equal, derived needs to check i
    virtual bool are_equal_(const holder_type& other) const = 0;

    /// The RTTI of the derived type
    rtti_type m_rtti_;
};

// -----------------------------------------------------------------------------
// Out of line inline implementations
// -----------------------------------------------------------------------------

template<concepts::WTFFloat FloatType>
void BufferViewHolder<FloatType>::assert_in_range(size_type index) const {
    if(index >= size()) {
        throw std::out_of_range("Index " + std::to_string(index) +
                                " is out of range for buffer of size " +
                                std::to_string(size()));
    }
}

} // namespace wtf::buffer::detail_
