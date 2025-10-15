#pragma once
#include <wtf/concepts/floating_point.hpp>
#include <wtf/rtti/type_holder.hpp>
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/is_convertible.hpp>
#include <wtf/types.hpp>

namespace wtf::rtti {

template<concepts::FloatingPoint T>
class TypeModel : public TypeHolder {
public:
    using holder_type = TypeHolder;

    using traits_type = wtf::type_traits::float_traits<T>;
    /// Unpack traits_type into the API
    ///@{
    using value_type       = typename traits_type::value_type;
    using const_value_type = typename traits_type::const_value_type;
    ///@}

    /// Pull types from base class to make them part of this class's API
    ///@{
    using string_type            = holder_type::string_type;
    using const_string_reference = holder_type::const_string_reference;
    ///@}

    explicit TypeModel(string_type name) : m_name_(std::move(name)) {
        m_qualified_name_ = traits_type::is_const ? "const " : "";
        m_qualified_name_ += m_name_;
        register_model_(*this);
    }

private:
    /// Implements TypeHolder::clone_ by making a new copy of *this
    TypeHolder* clone_() const override { return new TypeModel(*this); }

    /// Implements TypeHolder::name_ by returning the stored name
    const_string_reference name_() const override { return m_qualified_name_; }

    /// Implements TypeHolder::is_const_ using the traits class
    bool is_const_() const override { return traits_type::is_const; }

    /// Implements TypeHolder::make_const_ by making a const-qualified copy
    holder_pointer make_const_() const override {
        using const_type = std::add_const_t<T>;
        return get_model_(TypeModel<const_type>(m_name_).name());
    }

    /// Implements TypeHolder::are_equal_ by downcasting and comparing names
    bool are_equal_(const TypeHolder& other) const override {
        if(auto* p = dynamic_cast<const TypeModel*>(&other)) {
            return m_name_ == p->m_name_;
        }
        return false;
    }

    /// The unqualified name of the type being held
    string_type m_name_;

    /// The qualified name of the type being held
    string_type m_qualified_name_;
};

} // namespace wtf::rtti