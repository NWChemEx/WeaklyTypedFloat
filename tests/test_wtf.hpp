/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace test_wtf {

/// The types that C++20 by default considers floating point types
using default_fp_types = std::tuple<float, double, long double>;

/// Some types that C++20 by default does not consider floating point types
using not_fp_types = std::tuple<char, bool, std::string>;

/// To test custom floating point support we will define a simple FP type
class MyCustomFloat {
public:
    MyCustomFloat() : MyCustomFloat(0.0, "Zero initialized") {}
    MyCustomFloat(const MyCustomFloat&)            = default;
    MyCustomFloat(MyCustomFloat&&)                 = default;
    MyCustomFloat& operator=(const MyCustomFloat&) = default;
    MyCustomFloat& operator=(MyCustomFloat&&)      = default;
    MyCustomFloat(double v, std::string d) : value(v), desc(std::move(d)) {}

    bool operator==(const MyCustomFloat& other) const {
        return (value == other.value) && (desc == other.desc);
    }

private:
    double value;

    std::string desc;
};

} // namespace test_wtf