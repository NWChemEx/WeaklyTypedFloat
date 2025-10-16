#include <wtf/rtti/type_info.hpp>

namespace wtf::rtti {
using holder_reference       = TypeInfo::holder_reference;
using const_holder_reference = TypeInfo::const_holder_reference;
using const_string_reference = TypeInfo::const_string_reference;
using precision_type         = TypeInfo::precision_type;

TypeInfo::TypeInfo(holder_pointer holder) : m_holder_(std::move(holder)) {}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

const_string_reference TypeInfo::name() const noexcept {
    return holder_().name();
}

precision_type TypeInfo::precision() const noexcept {
    return holder_().precision();
}

// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

bool TypeInfo::operator==(const TypeInfo& other) const {
    return holder_().are_equal(other.holder_());
}

bool TypeInfo::operator!=(const TypeInfo& other) const {
    return !(*this == other);
}

// -----------------------------------------------------------------------------
// Private methods
// -----------------------------------------------------------------------------

holder_reference TypeInfo::holder_() { return *m_holder_; }

const_holder_reference TypeInfo::holder_() const { return *m_holder_; }

} // namespace wtf::rtti