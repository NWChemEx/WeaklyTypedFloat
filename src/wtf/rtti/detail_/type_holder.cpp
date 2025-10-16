#include <wtf/rtti/detail_/type_holder.hpp>
#include <wtf/rtti/detail_/type_model.hpp>

namespace wtf::rtti::detail_ {

using string_type    = TypeHolder::string_type;
using holder_pointer = TypeHolder::holder_pointer;

std::map<string_type, holder_pointer> TypeHolder::m_registry_;

} // namespace wtf::rtti::detail_