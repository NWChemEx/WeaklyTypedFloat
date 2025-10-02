#pragma once

#include <memory>
#include <type_traits>

namespace weakly_typed_float {

/**
 * @brief Type-erased floating point wrapper
 * 
 * This class provides type-erasure for floating point types, allowing you to
 * write generic floating-point code without worrying about the exact type.
 */
class WeaklyTypedFloat {
public:
    /**
     * @brief Default constructor - initializes to 0.0
     */
    WeaklyTypedFloat();

    /**
     * @brief Constructor from any floating point type
     * @tparam T The floating point type (float, double, long double)
     * @param value The initial value
     */
    template<typename T>
    WeaklyTypedFloat(T value, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr);

    /**
     * @brief Copy constructor
     */
    WeaklyTypedFloat(const WeaklyTypedFloat& other);

    /**
     * @brief Move constructor
     */
    WeaklyTypedFloat(WeaklyTypedFloat&& other) noexcept;

    /**
     * @brief Copy assignment operator
     */
    WeaklyTypedFloat& operator=(const WeaklyTypedFloat& other);

    /**
     * @brief Move assignment operator
     */
    WeaklyTypedFloat& operator=(WeaklyTypedFloat&& other) noexcept;

    /**
     * @brief Destructor
     */
    ~WeaklyTypedFloat();

    /**
     * @brief Convert to any floating point type
     * @tparam T The target floating point type
     * @return The value converted to type T
     */
    template<typename T>
    T as() const;

    /**
     * @brief Get value as double
     * @return The value as a double
     */
    double value() const;

    /**
     * @brief Addition operator
     */
    WeaklyTypedFloat operator+(const WeaklyTypedFloat& other) const;

    /**
     * @brief Subtraction operator
     */
    WeaklyTypedFloat operator-(const WeaklyTypedFloat& other) const;

    /**
     * @brief Multiplication operator
     */
    WeaklyTypedFloat operator*(const WeaklyTypedFloat& other) const;

    /**
     * @brief Division operator
     */
    WeaklyTypedFloat operator/(const WeaklyTypedFloat& other) const;

    /**
     * @brief Equality comparison
     */
    bool operator==(const WeaklyTypedFloat& other) const;

    /**
     * @brief Inequality comparison
     */
    bool operator!=(const WeaklyTypedFloat& other) const;

    /**
     * @brief Less than comparison
     */
    bool operator<(const WeaklyTypedFloat& other) const;

    /**
     * @brief Less than or equal comparison
     */
    bool operator<=(const WeaklyTypedFloat& other) const;

    /**
     * @brief Greater than comparison
     */
    bool operator>(const WeaklyTypedFloat& other) const;

    /**
     * @brief Greater than or equal comparison
     */
    bool operator>=(const WeaklyTypedFloat& other) const;

private:
    // Internal implementation using type erasure pattern
    class Concept {
    public:
        virtual ~Concept() = default;
        virtual std::unique_ptr<Concept> clone() const = 0;
        virtual double value() const = 0;
    };

    template<typename T>
    class Model : public Concept {
    public:
        explicit Model(T value) : m_value(value) {}

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model<T>>(m_value);
        }

        double value() const override {
            return static_cast<double>(m_value);
        }

    private:
        T m_value;
    };

    std::unique_ptr<Concept> m_pimpl;
};

// Template implementations

template<typename T>
WeaklyTypedFloat::WeaklyTypedFloat(T value, typename std::enable_if<std::is_floating_point<T>::value>::type*)
    : m_pimpl(std::make_unique<Model<T>>(value)) {}

template<typename T>
T WeaklyTypedFloat::as() const {
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
    return static_cast<T>(value());
}

} // namespace weakly_typed_float
