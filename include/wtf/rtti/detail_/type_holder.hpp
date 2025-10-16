#pragma once
#include <iostream>
#include <map>
#include <string>
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/dispatcher.hpp>
#include <wtf/type_traits/is_convertible.hpp>

namespace wtf::rtti::detail_ {

template<concepts::FloatingPoint T>
class TypeModel;

/** @brief Defines the interface for interacting with the type-erased type.
 *
 * This class provides a type-erased interface for interacting with the
 * floating-point type T held by TypeModel<T>. It provides methods for
 * interacting with the type without knowing what T is at compile time.
 */
class TypeHolder {
public:
    /// Type used for storing the name of the held type
    using string_type = std::string;

    /// Type used for passing around a read-only string_type without copying
    using const_string_reference = const string_type&;

    /// Type of pointer used to hold TypeHolder instances
    using holder_pointer = std::shared_ptr<TypeHolder>;

    /// Type used for specifying precision of floating-point types
    using precision_type = std::size_t;

    /// Default virtual destructor
    virtual ~TypeHolder() = default;

    //**************************************************************************
    // Accessors
    //**************************************************************************

    /** @brief Returns the model for the type of *this.
     *
     *  Types are singletons, so this method will always return the same
     *  instance of the model. The model is registered the first time a
     *  TypeModel<T> is constructed. The logic for finding the model is in the
     *  get_model_ method.
     *
     *  @return A shared pointer to the TypeHolder model for the type of *this.
     *
     *  @throws std::runtime_error if no model is registered for the type of
     *                             *this. This should be an impossible state.
     */
    holder_pointer get_model() const { return get_model_(name()); }

    /** @brief Returns a human-readable name for the type.
     *
     *  When a TypeModel<T> is constructed, it is given a name to represent the
     *  type. This name, plus any cv-qualifiers necessary, are applied to the
     *  name returned by this method. The logic for determining the name is in
     *  the private name_ method.
     *
     *  @return A string representing the name of the held type.
     *
     *  @throw None No throw guarantee.
     */
    const_string_reference name() const { return name_(); }

    /** @brief In base 10, the number of significant digits this type can hold.
     *
     *  The value returned by this method is determined by the
     *  type_traits::Precision class. See the documentation for that class for
     *  more information. The logic for determining the precision is in the
     *  private precision_ method.
     *
     *  @return The number of significant digits the held type can represent.
     *
     *  @throw None No throw guarantee.
     */
    precision_type precision() const { return precision_(); }

    /** @brief True if the held type is const-qualified and false otherwise.
     *
     *  The derived class uses template meta-programming to determine if the
     *  held type is const-qualified. This logic is implemented by the
     *  private is_const_ method.
     *
     *  @return True if the held type is const-qualified and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool is_const() const { return is_const_(); }

    /** @brief Returns a TypeHolder for the const version of the type held by
     *         *this.
     *
     *  If *this already holds a const-qualified type, this method returns
     *  the registered version of *this. Otherwise, it returns the registered
     *  version of the const-qualified type. The logic for this function is
     *  implemented by the private make_const_ method.
     *
     *  @return A shared pointer to a TypeHolder holding the const-qualified
     *          version of the type held by *this.
     *
     *  @throw std::runtime_error if no model is registered for the const-
     *                            version
     */
    holder_pointer make_const() const { return make_const_(); }

    /** @brief Determines if *this is polymorphically value equal to @p other.
     *
     *  Two TypeHolders are considered polymorphically equal if they hold the
     *  same qualified type and were declared with the same string name. The
     *  logic for this function is implemented by the private are_equal_ method.
     *
     *  @param[in] other The TypeHolder to compare *this to.
     *
     *  @return True if *this and @p other are polymorphically equal and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const TypeHolder& other) const { return are_equal_(other); }

    /** @brief Determines if the type held by *this can be implicitly converted
     *         to the type held by @p other.
     *
     *  @tparam TypeTuple A std::tuple of types to use for the conversion.
     *
     *  *this can be implicitly converted to @p other if, ignoring
     *  cv-qualifiers, an object of the type held by *this can be implicitly
     *  converted to an object of the type held by @p other. If *this is const-
     *  qualified we additionally enforce that @p other must be const-qualified.
     *
     *  @note This method is in general not symmetric.
     *
     *  @param[in] other The TypeHolder to check conversion to.
     *
     *  @return True if the type held by *this can be implicitly converted to
     *          the type held by @p other and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename TypeTuple>
    bool is_implicitly_convertible_to(const TypeHolder& other) const;

protected:
    /// Wraps the process of registering a possibly new TypeHolder
    void register_model_(const TypeHolder& model) {
        if(m_registry_.count(model.name())) return;
        m_registry_[model.name()] = holder_pointer(model.clone_());
    }

    /// Wraps the process of looking up a registered TypeHolder by name
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

    /// Derived class overwrites to provide the precision of the held type
    virtual precision_type precision_() const = 0;

    /// Derived class overwrites to provide whether the held type is const
    virtual bool is_const_() const = 0;

    /// Derived class overwrites to provide a const-qualified version of *this
    virtual holder_pointer make_const_() const = 0;

    /// Derived class overwrites to provide polymorphic equality comparison
    virtual bool are_equal_(const TypeHolder& other) const = 0;

    /// All registered TypeHolder models
    static std::map<std::string, std::shared_ptr<TypeHolder>> m_registry_;
};

// *****************************************************************************
// Inline method implementations
// *****************************************************************************

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
    return wtf::detail_::dispatch<TypeModel, TypeTuple>(lambda, *this, other);
}

} // namespace wtf::rtti::detail_