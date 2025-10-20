#pragma once
#include <wtf/buffer/detail_/buffer_holder.hpp>

namespace wtf {

class FloatBuffer {
public:
    /// Type defining the API for accessing the type-erased value
    using holder_type = detail_::BufferHolder;
};

} // namespace wtf