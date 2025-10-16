#pragma once
#include <memory>
#include <wtf/rtti/detail_/type_model.hpp>

namespace wtf::rtti {

/** @brief C++'s RTTI class is a bit lacking. Ours is better */
class TypeInfo {
public:
    using holder_type            = detail_::TypeHolder;
    using holder_pointer         = detail_::TypeHolder::holder_pointer;
    using holder_reference       = holder_type&;
    using const_holder_reference = const holder_type&;
    using precision_type         = detail_::TypeHolder::precision_type;

    explicit TypeInfo(holder_pointer holder) : m_holder_(std::move(holder)) {}

    /** @brief Can the type held by *this be implicitly converted to that held
     *         by @p other?
     *
     */
    template<typename TypeTuple>
    bool is_implicitly_convertible_to(const TypeInfo& other) const {
        const auto& holder2 = other.holder_();
        return holder_().is_implicitly_convertible_to<TypeTuple>(holder2);
    }

private:
    holder_reference holder_() { return *m_holder_; }

    const_holder_reference holder_() const { return *m_holder_; }

    holder_pointer m_holder_;
};

// template<typename T>
// TypeInfo wtf_typeid() {
//     TypeModel<T> model(typeid(T).name());
//     return TypeInfo(model.get_model());
// }

} // namespace wtf::rtti