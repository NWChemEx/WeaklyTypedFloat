#pragma once
#include <iostream>
#include <map>
#include <string>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/dispatcher.hpp>
#include <wtf/type_traits/is_convertible.hpp>

namespace wtf::rtti {

template<concepts::FloatingPoint T>
class TypeModel;

class TypeHolder {
public:
    using string_type            = std::string;
    using const_string_reference = const string_type&;
    using holder_pointer         = std::shared_ptr<TypeHolder>;

    virtual ~TypeHolder() = default;

    /** @brief Returns the model for the type of *this.
     *
     *  Types are singletons, so this method will always return the same
     *  instance of the model.
     *
     */
    holder_pointer get_model() const { return get_model_(name()); }

    const_string_reference name() const { return name_(); }

    bool is_const() const { return is_const_(); }

    holder_pointer make_const() const { return make_const_(); }

    bool are_equal(const TypeHolder& other) const { return are_equal_(other); }

    template<typename TypeTuple>
    bool is_implicitly_convertible_to(const TypeHolder& other) const;

protected:
    void register_model_(const TypeHolder& model) {
        if(m_registry_.count(model.name())) return;
        m_registry_[model.name()] = holder_pointer(model.clone_());
    }

    holder_pointer get_model_(const_string_reference name) const {
        if(m_registry_.count(name)) return m_registry_.at(name);
        throw std::runtime_error("No TypeHolder model registered for type " +
                                 std::string(name));
    }

private:
    /// Derived class overwrites to provide a deep copy of *this
    virtual TypeHolder* clone_() const = 0;

    /// Derived class overwrites to provide the name of the held type
    virtual const_string_reference name_() const = 0;

    /// Derived class overwrites to provide whether the held type is const
    virtual bool is_const_() const = 0;

    /// Derived class overwrites to provide a const-qualified version of *this
    virtual holder_pointer make_const_() const = 0;

    /// Derived class overwrites to provide polymorphic equality comparison
    virtual bool are_equal_(const TypeHolder& other) const = 0;

    /// All registered TypeHolder models
    static std::map<std::string, std::shared_ptr<TypeHolder>> m_registry_;
};

template<typename TypeTuple>
bool TypeHolder::is_implicitly_convertible_to(const TypeHolder& other) const {
    auto lambda = [](auto&& from, auto&& to) {
        using from_type = typename std::decay_t<decltype(from)>::value_type;
        using to_type   = typename std::decay_t<decltype(to)>::value_type;
        using clean_from_type = std::decay_t<from_type>;
        using clean_to_type   = std::decay_t<to_type>;
        return wtf::type_traits::is_convertible_v<clean_from_type,
                                                  clean_to_type>;
    };
    // Cannot convert away const-qualification
    if(is_const() && !other.is_const()) return false;
    return detail_::dispatch<TypeModel, TypeTuple>(lambda, *this, other);
}

} // namespace wtf::rtti