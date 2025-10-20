#pragma once
#include <wtf/concepts/wtf_float.hpp>
#include <wtf/float.hpp>

namespace wtf {

template<concepts::WTFFloat FloatType>
class FloatView {
public:
    /// The type of wtf::Float object *this is aliasing
    using float_type = FloatType;

    bool operator==(const FloatView& other) const {
        return m_float_ == other.m_float_;
    }
};

} // namespace wtf