#pragma once
#include <wtf/buffer/detail_/buffer_holder.hpp>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/type_traits/float_traits.hpp>

namespace wtf::buffer::detail_ {

/** @brief Type-erases a contiguous buffer of type FloatType*
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::FloatingPoint concept.
 *
 *  This class models a contiguous buffer of floating-point values of type
 *  @p FloatType.
 */
template<concepts::FloatingPoint FloatType>
class ContiguousModel : public BufferHolder {
public:
    /// Type *this inherits from
    using holder_type = BufferHolder;

    /// Pull in types from the base class
    ///@{
    using size_type       = typename holder_type::size_type;
    using view_type       = typename holder_type::view_type;
    using const_view_type = typename holder_type::const_view_type;
    ///@}

    /// Type defining the traits of @p FloatType
    using float_traits = type_traits::float_traits<FloatType>;

    /// Add types from float_traits to the API
    ///@{
    using unqualified_type = typename float_traits::unqualified_type;
    using pointer          = typename float_traits::pointer;
    using const_pointer    = typename float_traits::const_pointer;
    using reference        = typename float_traits::reference;
    using const_reference  = typename float_traits::const_reference;
    ///@}

    /// Type used to manage the floating-point buffer
    using vector_type = std::vector<FloatType>;

    /** @brief Takes ownership of @p buffer.
     *
     *  Internally ContiguousModel stores the buffer as an object of type
     *  vector_type. This ctor can be used to take ownership of an already
     *  exisiting buffer.
     *
     *  @param buffer The buffer to take ownership of.
     *
     *  @throw std::bad_alloc if creating the RTTI information fails. Strong
     *                        throw guarantee.
     */
    explicit ContiguousModel(vector_type buffer) :
      holder_type(rtti::wtf_typeid<unqualified_type>()),
      m_buffer_(std::move(buffer)) {}

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
        assert_in_range(idx);
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
        assert_in_range(idx);
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
     *  @param[in] other The other ContiguousModel to compare against.
     *
     *  @return true If the buffers are equal and false otherwise.
     *
     *  @throw None No-throw guarantee.
     */
    bool operator==(const ContiguousModel& other) const {
        return m_buffer_ == other.m_buffer_;
    }

private:
    /// Clones *this polymorphically
    holder_type* clone_() const override { return new ContiguousModel(*this); }

    /// Calls vector_type's operator[]
    view_type at_(size_type index) override {
        return view_type(m_buffer_[index]);
    }

    /// Calls vector_type's operator[]const
    const_view_type at_(size_type index) const override {
        return const_view_type(m_buffer_[index]);
    }

    /// Calls vector_type's size()
    size_type size_() const noexcept override { return m_buffer_.size(); }

    /// *this always store data contiguously (unless FloatType == bool)
    bool is_contiguous_() const override { return true; }

    /// Dispatches to operator==
    bool are_equal_(const holder_type& other) const override {
        if(auto pother = dynamic_cast<const ContiguousModel*>(&other)) {
            return *this == *pother;
        }
        return false;
    }

    /// The held buffer
    vector_type m_buffer_;
};

} // namespace wtf::buffer::detail_