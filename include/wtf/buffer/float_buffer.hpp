#pragma once
#include <span>
#include <wtf/buffer/detail_/contiguous_model.hpp>
#include <wtf/concepts/iterator.hpp>

namespace wtf::buffer {

/** @brief A type-erased buffer of floating-point values.
 *
 *  Conceptually this class type-erases a buffer (i.e. an array) of floating-
 *  point types. Think of the buffer more like std::array rather than
 *  std::vector. In particular, the size of FloatBuffer objects are fixed and
 *  can not be changed (aside from assigning a new FloatBuffer to them).
 */
class FloatBuffer {
public:
    /// Type defining the API for accessing the type-erased value
    using holder_type = detail_::BufferHolder;

    /// Pull in types from the holder_type
    ///@{
    using holder_pointer  = typename holder_type::holder_pointer;
    using size_type       = typename holder_type::size_type;
    using view_type       = typename holder_type::view_type;
    using const_view_type = typename holder_type::const_view_type;
    ///@}

    // -------------------------------------------------------------------------
    // Ctors and assignment operators
    // -------------------------------------------------------------------------

    /** @brief Creates a FloatBuffer from a std::vector.
     *
     *  @tparam T The type of floating-point value being held. Must satisfy
     *            the concepts::FloatingPoint concept.
     *
     *  In practice we find that many users store their floating-point data in
     *  std::vector objects. This ctor optimizes for this frequent use case by
     *  allowing the user to pass in a std::vector. If the user moves the
     *  vector into this ctor then no copies of the data will be made (if they
     *  do not move it then it will be copied).
     *
     *  @param[in] buffer The buffer to type-erase.
     *
     *  @throw std::bad_alloc if creating the held buffer fails. Strong throw
     *                        guarantee.
     */
    template<concepts::FloatingPoint T>
    explicit FloatBuffer(std::vector<T> buffer);

    /** @brief Creates a FloatBuffer from a range of iterators.
     *
     *  @tparam BeginItr The type of the begin iterator. Must satisfy the
     *                   concepts::FPIterator concept.
     *  @tparam EndItr The type of the end iterator. Must satisfy the
     *                 concepts::FPIterator concept.
     *
     *  This ctor will create a FloatBuffer by copying the elements in the
     *  range. Note that FloatBuffer objects own their data, so the data must
     *  be copied in. If the user would prefer to avoid the copy they may
     *  alias the data using the FloatBufferView class.
     *
     *  @param[in] begin The iterator pointing to the beginning of the range.
     *  @param[in] end The iterator pointing to one past the end of the range.
     *
     *  @throw std::bad_alloc if creating the held buffer fails. Strong throw
     *                        guarantee.
     */
    template<concepts::FPIterator BeginItr, concepts::FPIterator EndItr>
    FloatBuffer(BeginItr&& begin, EndItr&& end);

    /** @brief Creates a new FloatBuffer as a copy of @p other.
     *
     *  This ctor will initialize *this to hold a deep copy of @p other.
     *
     *  @param[in] other The FloatBuffer to copy from.
     *
     *  @throw std::bad_alloc if copying the held buffer fails. Strong throw
     *                        guarantee.
     */
    FloatBuffer(const FloatBuffer& other) :
      m_pholder_(other.m_pholder_->clone()) {}

    /** @brief Overrides the state of *this with a deep copy of @p other.
     *
     *  @param[in] other The FloatBuffer to copy from.
     *
     *  @return A reference to *this.
     *
     *  @throw std::bad_alloc if copying the held buffer fails. Strong throw
     *                        guarantee.
     */
    FloatBuffer& operator=(const FloatBuffer& other);

    /** @brief Creates a new FloatBuffer by taking the state from @p other.
     *
     *  This ctor will initialize *this by taking ownership of the buffer held
     *  in @p other.
     *
     *  @param[in,out] other The FloatBuffer to move from. After the operation
     *                       @p other is in a valid but unspecified state.
     *
     *  @throw No-throw guarantee.
     */
    FloatBuffer(FloatBuffer&& other) noexcept = default;

    /** @brief Overrides the state of *this with the state in @p other.
     *
     *  This operator will override the state of *this by taking ownership of
     *  the buffer held in @p other.
     *
     *  @param[in,out] other The FloatBuffer to move from. After the operation
     *                       @p other is in a valid but unspecified state.
     *
     *  @return A reference to *this.
     *
     *  @throw No-throw guarantee.
     */
    FloatBuffer& operator=(FloatBuffer&& other) noexcept = default;

    // -------------------------------------------------------------------------
    // State accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the element with offset @p index.
     *
     *  This method is used to retrieve an element of the buffer. The returned
     *  element aliases the buffer, meaning that you can write to the buffer
     *  through the element. While float_cast can be used to ge the address in
     *  the buffer (by casting to a reference), users should use
     *  `contiguous_buffer_cast` to get a span if they need to get the pointer
     *  (and ensure the underlying memory really is contiguous).
     *
     *  @param[in] index The index of the element to return.
     *
     *  @return A view to the element at offset @p index.
     *
     *  @throw std::out_of_range if @p index is not less than size().
     *                           Strong throw guarantee.
     */
    view_type at(size_type index) { return m_pholder_->at(index); }

    /** @brief Returns the element with offset @p index.
     *
     *  This method is the same as the non-const version, but returns a read-
     *  only view of the element. See the non-const version for more details.
     *
     *  @param[in] index The index of the element to return.
     *
     *  @return A const view to the element at offset @p index.
     *
     *  @throw std::out_of_range if @p index is not less than size(). Strong
     *                           throw guarantee.
     */
    const_view_type at(size_type index) const {
        return std::as_const(*m_pholder_).at(index);
    }

    /** @brief Returns the number of elements in the buffer.
     *
     *  This method is used to retrieve the number of elements in the buffer.
     *  Since the buffer is zero-indexed, valid indices for at() are in the
     *  range [0, size()).
     *
     *  @return The number of elements in the buffer.
     *
     *  @throw No-throw guarantee.
     */
    size_type size() const noexcept { return m_pholder_->size(); }

    /** @brief Does the held buffer store the values contiguously?
     *
     *  Given a pointer to one element in the buffer, if the elements of the
     *  buffer are contiguous then pointer arithmetic may be used to access the
     *  other elements. If the buffer is not contiguous then the only way to
     *  access the elements is through the at() method. Attempting to treat a
     *  non-contiguous buffer as contiguous will lead to undefined behavior.
     *
     *  @return true if the held buffer is contiguous, false otherwise.
     *
     *  @throw No-throw guarantee.
     */
    bool is_contiguous() const { return m_pholder_->is_contiguous(); }

    /** @brief Determines if *this is value equal to @p other.
     *
     *  Two FloatBuffer objects are value equal if they hold the same number
     *  of elements, if those elements are of the same type, and if the
     *  corresponding elements compare equal.
     *
     *  @param[in] other The FloatBuffer to compare against.
     *
     *  @return true if *this and @p other are value equal, false otherwise.
     *
     *  @throw No-throw guarantee.
     */
    bool operator==(const FloatBuffer& other) const {
        return m_pholder_->are_equal(*other.m_pholder_);
    }

    /** @brief Is this buffer different from @p other?
     *
     *  Two FloatBuffer objects are different if they are not value equal.
     *  Thus this method simply negates the result of operator==. See that
     *  method for more details.
     *
     *  @param[in] other The FloatBuffer to compare against.
     *
     *  @return true if *this and @p other are different, false otherwise.
     *
     *  @throw No-throw guarantee.
     */
    bool operator!=(const FloatBuffer& other) const {
        return !(*this == other);
    }

    template<typename T>
    std::span<T> value() {
        auto& model = downcast_<T>();
        return std::span<T>(model.data(), model.size());
    }

    template<typename T>
    std::span<const T> value() const {
        const auto& model = downcast_<T>();
        return std::span<const T>(model.data(), model.size());
    }

private:
    template<typename T>
    auto& downcast_() {
        using model_type = detail_::ContiguousModel<T>;
        auto pmodel      = dynamic_cast<model_type*>(m_pholder_.get());
        if(pmodel == nullptr) {
            throw std::runtime_error(
              "FloatBuffer does not hold the requested floating-point type");
        }

        return *pmodel;
    }

    template<typename T>
    const auto& downcast_() const {
        return const_cast<FloatBuffer*>(this)->downcast_<T>();
    }

    /// Other ctors create the holder and then dispatch to this ctor.
    FloatBuffer(holder_pointer pholder) : m_pholder_(std::move(pholder)) {}

    /// The held type-erased buffer
    holder_pointer m_pholder_;
};

/** @brief Wraps calling the FloatBuffer ctor.
 *
 *  @tparam Args The types of the arguments to forward to the FloatBuffer ctor.
 *
 *  This function is only provided so that FloatBuffer can be created by
 *  analogy to other type-erased objects like std::any, Float, and FloatView.
 *
 *  @param[in] args The arguments to forward to the FloatBuffer ctor.
 *
 *  @return A FloatBuffer constructed with the forwarded arguments.
 */
template<typename... Args>
auto make_float_buffer(Args&&... args) {
    return FloatBuffer(std::forward<Args>(args)...);
}

template<concepts::FloatingPoint T>
std::span<T> contiguous_buffer_cast(FloatBuffer& buffer) {
    if(!buffer.is_contiguous()) {
        throw std::runtime_error(
          "Cannot cast non-contiguous FloatBuffer to span");
    }
    return buffer.template value<T>();
}

// -----------------------------------------------------------------------------
// Out of line inline implementations
// -----------------------------------------------------------------------------

template<concepts::FloatingPoint T>
FloatBuffer::FloatBuffer(std::vector<T> buffer) :
  m_pholder_(std::make_unique<detail_::ContiguousModel<T>>(std::move(buffer))) {
}

template<concepts::FPIterator BeginItr, concepts::FPIterator EndItr>
FloatBuffer::FloatBuffer(BeginItr&& begin, EndItr&& end) :
  FloatBuffer(
    std::vector(std::forward<BeginItr>(begin), std::forward<EndItr>(end))) {}

FloatBuffer& FloatBuffer::operator=(const FloatBuffer& other) {
    if(this != &other) { m_pholder_ = other.m_pholder_->clone(); }
    return *this;
}

} // namespace wtf::buffer