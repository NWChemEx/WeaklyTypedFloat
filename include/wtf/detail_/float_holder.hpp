#pragma once

namespace wtf::detail_ {
class FloatHolder {
public:
    virtual ~FloatHolder() = default;

    bool are_equal(const FloatHolder& other) const { return are_equal_(other); }

    bool not_equal(const FloatHolder& other) const {
        return !are_equal_(other);
    }

private:
    virtual bool are_equal_(const FloatHolder& other) const = 0;
};

} // namespace wtf::detail_