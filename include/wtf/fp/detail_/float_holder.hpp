#pragma once
#include <wtf/rtti/type_info.hpp>

namespace wtf::fp::detail_ {

/** @brief Defines the interface used to interact with the type-erased float.
 *
 *  This class is the base class for all type-erased floating point holders.
 *  The FloatType class interacts with this class and then this class redirects
 *  the implementation to the typed derived class.
 */
class FloatHolder {
public:
    /// Type of *this
    using holder_type = FloatHolder;

    /// Type of a pointer to *this
    using holder_pointer = std::unique_ptr<holder_type>;

    /// Type of the RTTI container
    using type_info = rtti::TypeInfo;

    /// Read-only reference to type_info object
    using const_type_info_reference = const type_info&;

    /// Default virtual destructor
    virtual ~FloatHolder() = default;

    /** @brief Makes a deep polymorphic copy of *this.
     *
     *  This method is used to make deep copy of *this polymorphically. This
     *  means it will copy the state in any derived classes as well. This
     *  method is implemented by calling clone_().
     *
     *  @return A unique_ptr to a deep copy of *this.
     */
    holder_pointer clone() const { return holder_pointer(clone_()); }

    /** @brief Wraps the process of changing the held value.
     *
     *  This method is used to change the value held by *this to that held by
     *  @p other. The types of the held values must match and *this must not be
     *  holding a constant value.
     *
     *  @param[in] other The value to change to.
     *
     *  @throw std::invalid_argument if the type of the held value does not
     *                               match that of @p other. Strong throw
     *                               guarantee.
     */
    void change_value(const FloatHolder& other);

    /** @brief Determines if the object held by *this can be converted to the
     *         type in @p other.
     *
     *   @tparam TupleType A std::tuple of types that are allowed to be in the
     *                     holders.
     *
     *   This method simply calls rtti::is_implicitly_convertible with the
     *   wrapped RTTI objects. See the documentation for that function for more
     *   details.
     *
     *   @param[in] other The other FloatHolder to check convertibility against.
     *
     *   @return True if the type held by *this can be implicitly converted to
     *           the type held by @p other, false otherwise.
     *
     *   @throw None No throw guarantee.
     */
    template<typename TupleType>
    bool is_implicitly_convertible_to(const FloatHolder& other) {
        return rtti::is_implicitly_convertible<TupleType>(type(), other.type());
    }

    /** @brief Polymorphic value comparison.
     *
     *  This method determines if the value held by *this is exactly equal
     *  to that held by @p other. The comparison is done polymophically, i.e.,
     *  it will also ensure the state in the derived parts of *this and @p other
     *  are value equal.  This method is implemented by calling are_equal_.
     *
     *  @note The comparison will exactly compare the floating point values
     *        meaning they must be bitwise equal for built-in floating point
     *        types.
     *
     *  @param[in] other The other FloatHolder to compare against *this.
     *
     *  @return True if *this and @p other are polymorphically equal and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const FloatHolder& other) const {
        return type() == other.type() ? are_equal_(other) : false;
    }

    /** @brief Provides access to the RTTI of the derived type.
     *
     *  @return A read-only reference to the RTTI object for the type held by
     *          the derived class.
     *
     *  @throw None No throw guarantee.
     */
    const_type_info_reference type() const { return m_type_; }

protected:
    /// Initializes *this with the given type info object
    explicit FloatHolder(type_info ti) : m_type_(std::move(ti)) {}

private:
    /// Clones *this polymorphically
    virtual holder_type* clone_() const = 0;

    /// Base checked that types are equal, derived need only change values
    virtual void change_value_(const FloatHolder& other) = 0;

    /// Base checks that types are equal, derived need only check values
    virtual bool are_equal_(const FloatHolder& other) const = 0;

    /// The RTTI object for the object held by the derived class
    type_info m_type_;
};

} // namespace wtf::fp::detail_