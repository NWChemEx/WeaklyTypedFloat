#pragma once
#include <span>
#include <wtf/buffer/detail_/buffer_view_holder.hpp>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/fp/float.hpp>
#include <wtf/type_traits/float_traits.hpp>

namespace wtf::buffer::detail_ {

/** @brief Type-erases a contiguous buffer of type FloatType*
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::FloatingPoint concept.
 *
 *  This class aliases a contiguous buffer of floating-point values of type
 *  @p FloatType and models the type-erase interface defined by
 *  BufferViewHolder for the buffer.
 */
template<concepts::FloatingPoint FloatType>
class ContiguousViewModel
  : public BufferViewHolder<std::conditional_t<std::is_const_v<FloatType>,
                                               const fp::Float, fp::Float>> {
private:
    /// Type of *this
    using model_type = ContiguousViewModel<FloatType>;

    /// Type of a model type-erasing a read-only buffer
    using const_model_type = ContiguousViewModel<const FloatType>;

    /// Is *this aliasing a read-only buffer?
    static constexpr bool is_const = std::is_const_v<FloatType>;

    /// The type of buffer *this is acting like
    using buffer_type =
      std::conditional_t<is_const, const fp::Float, fp::Float>;

public:
    /// Type *this inherits from
    using holder_type = BufferViewHolder<buffer_type>;

    /// Pull in types from the base class
    ///@{
    using const_holder_type = typename holder_type::const_holder_type;
    using size_type         = typename holder_type::size_type;
    using view_type         = typename holder_type::view_type;
    using const_view_type   = typename holder_type::const_view_type;
    ///@}

    /// Type defining the traits of @p FloatType
    using float_traits = type_traits::float_traits<FloatType>;

    /// Add types from float_traits to the API
    ///@{
    using pointer         = typename float_traits::pointer;
    using const_pointer   = typename float_traits::const_pointer;
    using reference       = typename float_traits::reference;
    using const_reference = typename float_traits::const_reference;
    ///@}

    /// How *this stores the buffer internally
    using span_type = std::span<FloatType>;

    /** @brief Aliases the raw pointer @p pbuffer.
     *
     *  Internally ContiguousViewModel stores the buffer as a std::span. This
     *  ctor takes a pointer to where the buffer starts and the size of the
     *  buffer and uses that to create the span.
     *
     *  @param[in] pbuffer Pointer to the buffer to alias.
     *  @param[in] size The number of elements in the buffer.
     *
     *  @throw std::bad_alloc if creating the RTTI information fails. Strong
     *                        throw guarantee.
     */
    ContiguousViewModel(pointer pbuffer, size_type size) :
      ContiguousViewModel(span_type(pbuffer, size)) {}

    /** @brief Aliases the std::span @p buffer.
     *
     *  This ctor will alias the std::span @p buffer directly.
     *
     *  @param[in] buffer The buffer to alias.
     *
     *  @throw std::bad_alloc if creating the RTTI information fails. Strong
     *                        throw guarantee.
     */
    explicit ContiguousViewModel(span_type buffer) :
      holder_type(rtti::wtf_typeid<FloatType>()), m_buffer_(buffer) {}

    /** @brief Returns the typed element at index @p idx.
     *
     *  This method is similar to `at`, but returns the element as its actual
     *  type.
     *
     *  @param idx The index of the element to return.
     *
     *  @return A reference to the element at index @p idx.
     *
     *  @throw std::out_of_range if @p idx is out of range. Strong throw
     *                           guarantee.
     */
    reference get_element(size_type idx) {
        this->assert_in_range(idx);
        return m_buffer_[idx];
    }

    /** @brief Provides read-only access to an element.
     *
     *  This method is the same as the non-const version, but provides the
     *  element as a const reference.
     *
     *  @param[in] idx The index of the element to return.
     *
     *  @return A const reference to the element at index @p idx.
     *
     *  @throw std::out_of_range if @p idx is out of range. Strong throw
     *                           guarantee.
     */
    const_reference get_element(size_type idx) const {
        this->assert_in_range(idx);
        return m_buffer_[idx];
    }

    /** @brief Returns a mutable pointer to the underlying buffer.
     *
     *  @return A pointer to the underlying buffer.
     *
     *  @throw None No-throw guarantee.
     */
    pointer data() { return m_buffer_.data(); }

    /** @brief Returns a read-only pointer to the underlying buffer.
     *
     *  @return A const pointer to the underlying buffer.
     *
     *  @throw None No-throw guarantee.
     */
    const_pointer data() const { return m_buffer_.data(); }

    /** @brief Compares the elements in the buffer for exact equality.
     *
     *  Value equal is defined as having the same elements in the same order.
     *  Same elements means operator== returns true for each corresponding pair.
     *  For most floating-point types this means bitwise equality.
     *
     *  @param[in] other The other ContiguousViewModel to compare against.
     *
     *  @return true If the buffers are equal and false otherwise.
     *
     *  @throw None No-throw guarantee.
     */
    bool operator==(const ContiguousViewModel& other) const {
        if(this->size() != other.size()) return false;
        return std::equal(m_buffer_.begin(), m_buffer_.end(),
                          other.m_buffer_.begin());
    }

private:
    /// Calls copy ctor
    holder_type* clone_() const override {
        return new ContiguousViewModel(*this);
    }

    /// Calls span ctor of const model
    const_holder_type* const_clone_() const override {
        return new const_model_type(m_buffer_);
    }

    /// Calls span_type's operator[]
    view_type at_(size_type index) override {
        return view_type(m_buffer_[index]);
    }

    /// Calls span_type's operator[]const
    const_view_type at_(size_type index) const override {
        return const_view_type(m_buffer_[index]);
    }

    /// Calls span_type's size()
    size_type size_() const noexcept override { return m_buffer_.size(); }

    /// *this always store data contiguously
    bool is_contiguous_() const override { return true; }

    /// Dispatches to operator==
    bool are_equal_(const holder_type& other) const override {
        if(auto pother = dynamic_cast<const ContiguousViewModel*>(&other)) {
            return *this == *pother;
        }
        return false;
    }

    /// The held buffer
    span_type m_buffer_;
};

} // namespace wtf::buffer::detail_