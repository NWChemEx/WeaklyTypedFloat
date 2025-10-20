#pragma once
#include <wtf/buffer/detail_/buffer_holder.hpp>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/type_traits/float_traits.hpp>

namespace wtf::detail_ {

/** @brief Type-erases a contiguous buffer of type FloatType*
 *
 *  @tparam FloatType The type of floating-point value being held. Must satisfy
 *                    the concepts::UnmodifiedFloatingPoint concept.
 */
template<concepts::FloatingPoint FloatType>
class ContiguousModel : public BufferHolder {
public:
    /// Type *this inherits from
    using holder_type = BufferHolder;

    /// Pull in types from the base class
    ///@{
    using size_type = typename holder_type::size_type;
    ///@}

    /// Type defining the traits of @p FloatType
    using float_traits = type_traits::float_traits<FloatType>;

    /// Add types from float_traits to the API
    ///@{
    using unqualified_type = typename float_traits::unqualified_type;
    using pointer          = typename float_traits::pointer;
    using reference        = typename float_traits::reference;
    ///@}

    /// Type used to manage the floating-point buffer
    using vector_type = std::vector<FloatType>;

    explicit ContiguousModel(vector_type buffer) :
      holder_type(rtti::wtf_typeid<unqualified_type>()),
      m_buffer_(std::move(buffer)) {}

    reference get_element(size_type idx) { return m_buffer_.at(idx); }

    pointer data() { return m_buffer_.data(); }

    bool operator==(const ContiguousModel& other) const {
        return m_buffer_ == other.m_buffer_;
    }

private:
    /// Clones *this polymorphically
    holder_type* clone_() const override { return new ContiguousModel(*this); }

    /// *this always store data contiguously
    bool is_contiguous_() const override { return true; }

    bool are_equal_(const holder_type& other) const override {
        if(auto pother = dynamic_cast<const ContiguousModel*>(&other)) {
            return *this == *pother;
        }
        return false;
    }

    /// The held buffer
    vector_type m_buffer_;
};

} // namespace wtf::detail_