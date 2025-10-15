#include <wtf/rtti/type_holder.hpp>
#include <wtf/rtti/type_model.hpp>

namespace wtf::rtti {

using string_type    = TypeHolder::string_type;
using holder_pointer = TypeHolder::holder_pointer;

std::map<string_type, holder_pointer> TypeHolder::m_registry_;

} // namespace wtf::rtti