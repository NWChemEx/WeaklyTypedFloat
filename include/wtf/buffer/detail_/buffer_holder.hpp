#pragma once
#include <memory>
#include <wtf/fp/float_view.hpp>
#include <wtf/rtti/type_info.hpp>

namespace wtf::detail_ {

class BufferHolder {
public:
    /// Type of *this
    using holder_type = BufferHolder;

    /// Type of a pointer to *this
    using holder_pointer = std::unique_ptr<holder_type>;

    /// Type used to describe the RTTI of the held buffer
    using rtti_type = rtti::TypeInfo;

    /// Read-only reference to the RTTI of the held buffer
    using const_rtti_reference = const rtti_type&;

    /// Type used for indexing
    using size_type = std::size_t;

    /// Default virtual destructor
    virtual ~BufferHolder() = default;

    /** @brief Makes a deep polymorphic copy of *this.
     *
     *  This method is used to make deep copy of *this polymorphically. This
     *  means it will copy the state in any derived classes as well. This
     *  method is implemented by calling clone_().
     *
     *  @return A unique_ptr to a deep copy of *this.
     */
    holder_pointer clone() const { return holder_pointer(clone_()); }

    const_rtti_reference type() const { return m_rtti_; }

    bool is_contiguous() const { return is_contiguous_(); }

    bool are_equal(const holder_type& other) const {
        return m_rtti_ == other.m_rtti_ ? are_equal_(other) : false;
    }

protected:
    /// Used by derived class to initialize *this
    explicit BufferHolder(rtti_type rtti) : m_rtti_(std::move(rtti)) {}

private:
    /// Clones *this polymorphically
    virtual holder_type* clone_() const = 0;

    /// Does the derived class store the buffer contiguously in memory?
    virtual bool is_contiguous_() const = 0;

    /// RTTI is equal, derived needs to check i
    virtual bool are_equal_(const holder_type& other) const = 0;

    /// The RTTI of the derived type
    rtti_type m_rtti_;
};

} // namespace wtf::detail_