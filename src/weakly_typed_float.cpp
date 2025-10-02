#include "weakly_typed_float/weakly_typed_float.hpp"

namespace weakly_typed_float {

// Constructors and destructor
WeaklyTypedFloat::WeaklyTypedFloat() 
    : m_pimpl(std::make_unique<Model<double>>(0.0)) {}

WeaklyTypedFloat::WeaklyTypedFloat(const WeaklyTypedFloat& other)
    : m_pimpl(other.m_pimpl->clone()) {}

WeaklyTypedFloat::WeaklyTypedFloat(WeaklyTypedFloat&& other) noexcept = default;

WeaklyTypedFloat& WeaklyTypedFloat::operator=(const WeaklyTypedFloat& other) {
    if (this != &other) {
        m_pimpl = other.m_pimpl->clone();
    }
    return *this;
}

WeaklyTypedFloat& WeaklyTypedFloat::operator=(WeaklyTypedFloat&& other) noexcept = default;

WeaklyTypedFloat::~WeaklyTypedFloat() = default;

// Value extraction
double WeaklyTypedFloat::value() const {
    return m_pimpl->value();
}

// Arithmetic operators
WeaklyTypedFloat WeaklyTypedFloat::operator+(const WeaklyTypedFloat& other) const {
    return WeaklyTypedFloat(value() + other.value());
}

WeaklyTypedFloat WeaklyTypedFloat::operator-(const WeaklyTypedFloat& other) const {
    return WeaklyTypedFloat(value() - other.value());
}

WeaklyTypedFloat WeaklyTypedFloat::operator*(const WeaklyTypedFloat& other) const {
    return WeaklyTypedFloat(value() * other.value());
}

WeaklyTypedFloat WeaklyTypedFloat::operator/(const WeaklyTypedFloat& other) const {
    return WeaklyTypedFloat(value() / other.value());
}

// Comparison operators
bool WeaklyTypedFloat::operator==(const WeaklyTypedFloat& other) const {
    return value() == other.value();
}

bool WeaklyTypedFloat::operator!=(const WeaklyTypedFloat& other) const {
    return value() != other.value();
}

bool WeaklyTypedFloat::operator<(const WeaklyTypedFloat& other) const {
    return value() < other.value();
}

bool WeaklyTypedFloat::operator<=(const WeaklyTypedFloat& other) const {
    return value() <= other.value();
}

bool WeaklyTypedFloat::operator>(const WeaklyTypedFloat& other) const {
    return value() > other.value();
}

bool WeaklyTypedFloat::operator>=(const WeaklyTypedFloat& other) const {
    return value() >= other.value();
}

} // namespace weakly_typed_float
