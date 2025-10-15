#pragma once
#include <memory>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/float_model.hpp>

namespace wtf {

/** @brief Type-erases a floating-point number.
 *
 *  Notes on const-ness.
 *
 *  When a Float is created it will own the value it was given. Thus the holder
 *  always has the ability to modify the value. However, if the Float object is
 *  const qualified, then the value cannot be modified through the Float API.
 */
class Float {
public:
    using holder_type    = detail_::FloatHolder;
    using holder_pointer = std::unique_ptr<holder_type>;

    explicit Float(holder_pointer holder) : m_holder_(std::move(holder)) {}

private:
    template<typename T>
        requires concepts::FloatingPoint<std::decay_t<T>>
    friend T float_cast(Float& f);

    holder_pointer m_holder_;
};

template<concepts::FloatingPoint T>
Float make_float(T value) {
    return Float(std::make_unique<detail_::FloatModel<T>>(std::move(value)));
}

template<typename T>
    requires concepts::FloatingPoint<std::decay_t<T>>
T float_cast(Float& f) {
    auto* p            = f.m_holder_.get();
    using derived_type = detail_::FloatModel<std::decay_t<T>>;
    auto pderived      = dynamic_cast<derived_type*>(p);
    if(pderived == nullptr) {
        throw std::runtime_error("wtf::float_cast: bad cast");
    }
    return *pderived->data();
}
} // namespace wtf