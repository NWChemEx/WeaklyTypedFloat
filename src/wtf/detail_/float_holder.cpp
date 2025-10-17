#include <wtf/detail_/float_holder.hpp>

namespace wtf::detail_ {

void FloatHolder::change_value(const FloatHolder& other) {
    if(type() == other.type()) {
        change_value_(other);
    } else {
        throw std::invalid_argument(
          "FloatHolder::set_value: Types do not match");
    }
}

} // namespace wtf::detail_