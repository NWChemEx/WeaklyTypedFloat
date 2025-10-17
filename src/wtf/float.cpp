#include <wtf/float.hpp>

namespace wtf {

Float& Float::operator=(const Float& other) {
    if(this != &other) { m_holder_ = other.m_holder_->clone(); }
    return *this;
}

} // namespace wtf