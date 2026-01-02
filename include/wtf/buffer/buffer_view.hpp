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
#include <span>
#include <wtf/buffer/detail_/contiguous_view_model.hpp>
#include <wtf/concepts/float_buffer.hpp>
#include <wtf/concepts/iterator.hpp>
#include <wtf/concepts/wtf_float.hpp>

namespace wtf::buffer {

/** @brief A type-erased buffer of floating-point values.
 *
 *  @tparam FloatType The type of the elements in the buffer. Must satisfy the
 *                     concepts::WTFFloat concept.
 *
 *  This class aliases an existing buffer and allows the user to interact with
 *  it as if it were a FloatBuffer object.
 */
template<concepts::WTFFloat FloatType>
class BufferView {
private:
    /// Is *this aliasing a read-only buffer?
    static constexpr bool is_const = std::is_const_v<FloatType>;

    /// Applies const-qualification based on is_const
    template<typename T>
    using apply_const_t = std::conditional_t<is_const, const T, T>;

    /// Type of the model when it aliases a contiguous buffer of type @p T
    template<concepts::FloatingPoint T>
    using contiguous_model_type =
      detail_::ContiguousViewModel<apply_const_t<T>>;

public:
    /// Type defining the API for accessing the type-erased value
    using holder_type = detail_::BufferViewHolder<apply_const_t<FloatType>>;

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

    /** @brief Creates a view of a null buffer.
     *
     *  A null buffer has zero elements and can not be accessed. This ctor is
     *  used to create a null buffer.
     *
     *  @throw None No-throw guarantee.
     */
    BufferView() noexcept : BufferView(nullptr) {}

    /** @brief Implicitly converts FloatBuffer to BufferView.
     *
     *  @tparam BufferType2 The type of the buffer being converted from. Must
     *                      satisfy the concept FloatBuffer.
     *
     *  This ctor is used to implicitly convert FloatBuffer objects into
     *  BufferView objects.
     *
     *  @param[in] buffer The buffer to convert from.
     *
     *  @throw std::bad_alloc if allocating the holder fails. Strong throw
     *                        guarantee.
     */
    template<typename BufferType2>
        requires(concepts::FloatBuffer<std::remove_reference_t<BufferType2>>)
    BufferView(BufferType2&& buffer) : BufferView(buffer.as_view()) {}

    /** @brief Creates a BufferView from an existing buffer.
     *
     *  @tparam T The type of floating-point value being held. Must satisfy
     *            the concepts::FloatingPoint concept.
     *
     *  @param[in] pbuffer A pointer to the first element in the buffer.
     *  @param[in] size    The number of elements in the buffer.
     *
     *  @throw std::bad_alloc if allocating the holder fails. Strong throw
     *                        guarantee.
     */
    template<typename T>
        requires(concepts::FloatingPoint<T>)
    explicit BufferView(T* pbuffer, size_type size) :
      m_pholder_(std::make_unique<contiguous_model_type<T>>(pbuffer, size)){};

    /// Other ctors create the holder and then dispatch to this ctor.
    BufferView(holder_pointer pholder) : m_pholder_(std::move(pholder)) {}

    /** @brief Conversion from mutable alias to a read-only alias.
     *
     *  @tparam OtherFloat The type of floating-point value being held in
     *                     @p other. Must satisfy the concepts::WTFFloat
     *                     concept.
     *
     *  This ctor enables implicit conversion from a BufferView<fp::Float> to
     *  BufferView<const fp::Float>.
     *
     *  @param[in] other The BufferView to convert.
     *
     *  @throw std::bad_alloc if allocating the holder fails. Strong throw
     *                        guarantee.
     */
    template<concepts::WTFFloat OtherFloat>
        requires(is_const && !concepts::ConstQualified<OtherFloat>)
    BufferView(const BufferView<OtherFloat>& other) :
      m_pholder_(other.is_holding_() ? other.m_pholder_->const_clone() :
                                       nullptr) {}

    /** @brief Creates a new BufferView as a copy of @p other.
     *
     *  This ctor will initialize *this to hold a shallow copy of @p other.
     *  Here "shallow" means that *this will alias the same buffer as @p other.
     *  Note that *this will NOT alias @p other itself, i.e., modifying the
     *  what *this aliases will not modify what @p other aliases.
     *
     *  @param[in] other The BufferView to copy from.
     *
     *  @throw std::bad_alloc if copying the held buffer fails. Strong throw
     *                        guarantee.
     */
    BufferView(const BufferView& other) :
      m_pholder_(other.is_holding_() ? other.m_pholder_->clone() : nullptr) {}

    /** @brief Overrides the state of *this with a shallow copy of @p other.
     *
     *  This operator is similar to the copy ctor, except that the copy is
     *  being assigned to a pre-existing BufferView. The state in *this will be
     *  overridden by a shallow copy of @p other.
     *
     *  @param[in] other The BufferView to copy from.
     *
     *  @return A reference to *this.
     *
     *  @throw std::bad_alloc if copying the held buffer fails. Strong throw
     *                        guarantee.
     */
    BufferView& operator=(const BufferView& other) {
        if(this != &other) {
            if(other.is_holding_())
                m_pholder_ = other.m_pholder_->clone();
            else
                m_pholder_.reset();
        }
        return *this;
    }

    /** @brief Creates a new BufferView by taking the state from @p other.
     *
     *  This ctor will initialize *this by taking ownership of the state
     *  in @p other. After the operation @p other is in a valid but unspecified
     *  state.
     *
     *  @param[in,out] other The BufferView to move from. After the operation
     *                       @p other is in a valid but unspecified state.
     *
     *  @throw No-throw guarantee.
     */
    BufferView(BufferView&& other) noexcept = default;

    /** @brief Overrides the state of *this with the state in @p other.
     *
     *  This operator is the same as the move ctor, except that the move will
     *  override the state in *this instead of initializing it.
     *
     *  @param[in,out] other The BufferView to move from. After the operation
     *                       @p other is in a valid but unspecified state.
     *
     *  @return A reference to *this.
     *
     *  @throw No-throw guarantee.
     */
    BufferView& operator=(BufferView&& other) noexcept = default;

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
    view_type at(size_type index) {
        valid_index_(index);
        return m_pholder_->at(index);
    }

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
        valid_index_(index);
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
    size_type size() const noexcept {
        return is_holding_() ? m_pholder_->size() : 0;
    }

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
    bool is_contiguous() const {
        return is_holding_() ? m_pholder_->is_contiguous() : true;
    }

    /** @brief Determines if *this is value equal to @p other.
     *
     *  Two BufferView objects are value equal if they hold the same number
     *  of elements, if those elements are of the same type, and if the
     *  corresponding elements compare equal.
     *
     *  @param[in] other The BufferView to compare against.
     *
     *  @return true if *this and @p other are value equal, false otherwise.
     *
     *  @throw No-throw guarantee.
     */
    bool operator==(const BufferView& other) const {
        if(!is_holding_() && !other.is_holding_()) return true;
        if(!is_holding_() || !other.is_holding_())
            return size() == other.size();

        // Both are holding buffers, so compare the buffers
        return m_pholder_->are_equal(*other.m_pholder_);
    }

    /** @brief Determines if *this is value equal to @p other.
     *
     *  This overload allows comparison between BufferView objects that alias
     *  buffers of different const-ness. See the other operator== for details on
     *  what value equality means. The actual comparison is done by converting
     *  both *this and @p other to const holders and then comparing those.
     *
     *  @param[in] other The BufferView to compare against.
     *
     * @return true if *this and @p other are value equal, false otherwise.
     *
     * @throw std::bad_alloc if allocating the temporary holder fails. Strong
     *                       throw guarantee.
     */
    template<concepts::WTFFloat OtherFloat>
        requires(!std::is_same_v<OtherFloat, FloatType>)
    bool operator==(const BufferView<OtherFloat>& other) const {
        if(!is_holding_() && !other.is_holding_()) return true;
        if(!is_holding_() || !other.is_holding_())
            return size() == other.size();

        // Both are holding buffers, so compare the buffers
        auto plhs = m_pholder_->const_clone();
        auto prhs = other.m_pholder_->const_clone();
        return plhs->are_equal(*prhs);
    }

    /** @brief Is this buffer different from @p other?
     *
     *  @tparam OtherFloat The type of floating-point value being held in
     *                     @p other. Must satisfy the concepts::WTFFloat
     *                     concept.
     *
     *  Two BufferView objects are different if they are not value equal.
     *  Thus this method simply negates the result of operator==. See that
     *  method for more details.
     *
     *  @param[in] other The BufferView to compare against.
     *
     *  @return true if *this and @p other are different, false otherwise.
     *
     *  @throw No-throw guarantee.
     */
    template<concepts::WTFFloat OtherFloat>
    bool operator!=(const BufferView<OtherFloat>& other) const {
        return !(*this == other);
    }

    /** @brief Used to access the buffer.
     *
     *  @tparam T The type of floating-point value being held. Must satisfy
     *            the concepts::FloatingPoint concept.
     *
     *  Users are encouraged to use `contiguous_buffer_cast` instead of this
     *  method. This method is exposed to facilitate implementing that function.
     *
     *  @return A span aliasing the held buffer.
     *
     *  @throw std::runtime_error if the held buffer is not of type T. Strong
     *                            throw guarantee.
     */
    template<typename T>
    std::span<T> value() {
        if(!is_holding_()) return {};
        auto& model = downcast_<T>();
        return std::span<T>(model.data(), model.size());
    }

    /** @brief Used to access the buffer.
     *
     *  @tparam T The type of floating-point value being held. Must satisfy
     *             the concepts::FloatingPoint concept.
     *
     *  This method is the same as the non-const version, but returns a read-
     *  only span. Users are encouraged to use `contiguous_buffer_cast`
     *  instead of this method. This method is exposed to facilitate
     *  implementing that function.
     *
     *  @return A const span aliasing the held buffer.
     *
     *  @throw std::runtime_error if the held buffer is not of type T. Strong
     *                          throw guarantee.
     */
    template<typename T>
    std::span<const T> value() const {
        if(!is_holding_()) return {};
        const auto& model = downcast_<T>();
        return std::span<const T>(model.data(), model.size());
    }

private:
    template<concepts::WTFFloat Float2>
    friend class BufferView;

    template<typename TupleType, typename Visitor, typename... Args>
    friend auto visit_contiguous_buffer_view(Visitor&& visitor, Args&&... args);

    holder_type& holder_() { return *m_pholder_; }

    const holder_type& holder_() const { return *m_pholder_; }

    void valid_index_(size_type index) const {
        if(index >= size()) {
            throw std::out_of_range("BufferView: index out of range");
        }
    }

    /// Wraps converting to a contiguous model
    template<typename T>
    auto& downcast_() {
        using model_type = contiguous_model_type<T>;
        auto pmodel      = dynamic_cast<model_type*>(m_pholder_.get());
        if(pmodel == nullptr) {
            throw std::runtime_error(
              "BufferView does not hold the requested floating-point type");
        }

        return *pmodel;
    }

    /// Wraps converting to a read-only contiguous model
    template<typename T>
    const auto& downcast_() const {
        return const_cast<BufferView*>(this)->downcast_<T>();
    }

    /// True if *this is aliasing a buffer and false otherwise
    bool is_holding_() const noexcept { return m_pholder_ != nullptr; }

    /// The held type-erased buffer
    holder_pointer m_pholder_;
};

/** @brief Deduction guide for BufferView.
 *
 *  @tparam T The type of floating-point value being held. Must satisfy
 *            the concepts::FloatingPoint concept.
 *
 *  This deduction guide allows BufferView to be created without specifying
 *  the FloatType template parameter explicitly.
 *
 *  @param[in] pbuffer A pointer to the first element in the buffer.
 *  @param[in] size    The number of elements in the buffer.
 */
template<typename T>
    requires(concepts::FloatingPoint<T>)
BufferView(T*, std::size_t) -> BufferView<
  std::conditional_t<std::is_const_v<T>, const fp::Float, fp::Float>>;

/** @brief Wraps calling the BufferView ctor.
 *
 *  @related BufferView
 *
 *  @tparam Args The types of the arguments to forward to the BufferView ctor.
 *
 *  This function is only provided so that BufferView can be created by
 *  analogy to other type-erased objects like std::any, Float, and FloatView.
 *
 *  @param[in] args The arguments to forward to the BufferView ctor.
 *
 *  @return A BufferView constructed with the forwarded arguments.
 */
template<typename... Args>
auto make_buffer_view(Args&&... args) {
    return BufferView(std::forward<Args>(args)...);
}

/** @brief Used to retrieve the raw contiguous memory from a buffer view.
 *
 *  @related BufferView
 *
 *  @tparam T The type of floating-point value being held. Must satisfy
 *            the concepts::FloatingPoint concept. The user must explicitly
 *            provide this template type parameter.
 *  @tparam FloatType The type of buffer the BufferView is acting like. Must
 *                    satisfy the concepts::WTFFloat concept. The compiler
 *                    will deduce this parameter.
 *
 *  This function is the preferred way to get the typed contiguous memory back
 *  from a BufferView. It will check that the held buffer is contiguous.
 *
 *  @param[in] buffer The BufferView to get the contiguous memory from.
 *
 *  @return A span aliasing the contiguous memory held by @p buffer.
 *
 *  @throw std::runtime_error if the held buffer is not contiguous. Strong
 *                            throw guarantee.
 *  @throw std::runtime_error if the held buffer is not of type T. Strong
 *                           throw guarantee.
 */
template<concepts::FloatingPoint T, concepts::WTFFloat FloatType>
std::span<T> contiguous_buffer_cast(BufferView<FloatType>& buffer) {
    if(!buffer.is_contiguous()) {
        throw std::runtime_error(
          "Cannot cast non-contiguous BufferView to span");
    }
    return buffer.template value<T>();
}

/** @brief Wraps the process of calling a visitor with zero or more
 *         BufferView objects.
 *
 *  @relates BufferView
 *
 *  @tparam TupleType A std::tuple of floating-point types to try. Must be
 *                   explicitly provided by the user.
 *  @tparam Visitor The type of the visitor to call. Must be a callable object
 *                  capable of accepting `std::span<T>` objects for each
 *                  possible T in @p TupleType. Will be inferred by the
 *                  compiler.
 *  @tparam Args The types of the arguments to forward to the visitor. Each
 *               is expected to be downcastable to a ContiguousModel holding
 *               one of the types in @p TupleType. Will be inferred by the
 *               compiler.
 *
 *  @param[in] visitor The visitor to call with the unwrapped std::span<T>
 *                     objects.
 *  @param[in] args The ContiguousModel objects to unwrap and pass to the
 *                  visitor.
 *
 *  @return The return value of calling @p visitor with the unwrapped
 *          std::span<T> objects.
 *
 *  @throw std::runtime_error if any of the @p args cannot be downcast to a
 *                            ContiguousModel holding one of the types in
 *                            @p TupleType. Strong throw guarantee.
 *  @throw ??? if calling @p visitor throws. Same throw guarantee.
 */
template<typename TupleType, typename Visitor, typename... Args>
auto visit_contiguous_buffer_view(Visitor&& visitor, Args&&... args) {
    return detail_::visit_contiguous_view_model<TupleType>(
      std::forward<Visitor>(visitor), args.holder_()...);
}

} // namespace wtf::buffer
