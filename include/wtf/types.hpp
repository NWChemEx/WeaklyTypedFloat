#pragma once
#include <tuple>

namespace wtf {

/// A tuple holding C++'s default, real, floating-point types
using default_fp_types = std::tuple<float, double, long double>;
} // namespace wtf