/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <wtf/concepts/floating_point.hpp>
#include <wtf/rtti/detail_/type_holder.hpp>
#include <wtf/type_traits/float_traits.hpp>
#include <wtf/type_traits/precision.hpp>
#include <wtf/types.hpp>

namespace wtf::rtti::detail_ {

/** @brief Implements TypeHolder for type @p T.
 *
 *  @tparam T A floating-point type, possibly const-qualified.
 *
 * This class implements the TypeHolder interface for a specific floating-point
 * type T.
 */
template<concepts::FloatingPoint T>
class TypeModel : public TypeHolder {
public:
    /// Type providing the type-erased interface used by the interface
    using holder_type = TypeHolder;

    /// Type providing traits for the held type T
    using traits_type = wtf::type_traits::float_traits<T>;

    /// Unpack traits_type into the API
    ///@{
    using value_type       = typename traits_type::value_type;
    using unqualified_type = typename traits_type::unqualified_type;
    using const_value_type = typename traits_type::const_value_type;
    ///@}

    /// Pull types from base class to make them part of this class's API
    ///@{
    using string_type            = holder_type::string_type;
    using const_string_reference = holder_type::const_string_reference;
    ///@}

    /** @brief Creates a model with human-readable name @p name.
     *
     *  @param name The unqualified name of the type T, e.g. "float" or
     *              "double".
     */
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

    /// Implements TypeHolder::precision_
    precision_type precision_() const override {
        return type_traits::precision_v<unqualified_type>;
    }

    /// Implements TypeHolder::is_const_ using the traits class
    bool is_const_() const override { return traits_type::is_const; }

    /// Implements TypeHolder::make_const_ by making a const-qualified copy
    holder_pointer make_const_() const override {
        return get_model_(TypeModel<const_value_type>(m_name_).name());
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

} // namespace wtf::rtti::detail_
