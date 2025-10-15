#pragma once
#include <memory>
#include <wtf/rtti/type_model.hpp>

namespace wtf::rtti {

/** @brief C++'s RTTI class is a bit lacking. Ours is better */
class TypeInfo {
public:
    using holder_type    = TypeHolder;
    using holder_pointer = std::shared_ptr<holder_type>;

    explicit TypeInfo(holder_pointer holder) : m_holder_(std::move(holder)) {}

    /** @brief Can the type held by *this be implicitly converted to that held
     *         by @p other?
     *
     */
    template<typename TypeTuple>
    bool is_implicitly_convertible_to(const TypeHolder& other) const {
        return m_holder_->is_implicitly_convertible_to<TypeTuple>(other);
    }

private:
    holder_pointer m_holder_;
};

// template<typename T>
// TypeInfo wtf_typeid() {
//     TypeModel<T> model(typeid(T).name());
//     return TypeInfo(model.get_model());
// }

} // namespace wtf::rtti