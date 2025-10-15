#pragma once

namespace wtf::detail_ {
class FloatHolder {
public:
    virtual ~FloatHolder() = default;

    /** @brief Determines if the object held by *this can be converted to the
     *         type in @p other.
     */
    bool is_convertible_to(const FloatHolder& other) { return true; }

    bool are_equal(const FloatHolder& other) const { return are_equal_(other); }

    bool not_equal(const FloatHolder& other) const {
        return !are_equal_(other);
    }

private:
    virtual bool are_equal_(const FloatHolder& other) const = 0;
};

} // namespace wtf::detail_