/*
 * Copyright 2026 NWChemEx-Project
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

namespace wtf::fp {

/** @brief Code factorization for Float and FloatView.
 *
 *  @tparam DerivedType The derived type *this implements, either Float or
 *                      FloatView.
 *
 *  By design Float and FloatView share a lot of code. This class is used to
 *  consolidate that shared code in one place.
 *
 *  @note This class was added late in the initial development and so it does
 *        not contain all of the factorized code. Migrating the missing code
 *        here should be considered tech debt that needs to be repaid.
 */
template<typename DerivedType>
class FloatBase {
public:
    /// Type describing the RTTI information
    using rtti_type = rtti::TypeInfo;

    /** @brief Returns the RTTI of the held float.
     *
     *  This method returns the RTTI information for the floating-point type.
     *  If *this does not hold a value, the RTTI information will describe a
     *  nullptr_t type.
     *
     *  @return The RTTI for the held floating-point type.
     *
     *  @throw std::bad_alloc if there is a problem creating the type info.
     *                        Strong throw guarantee.
     */
    rtti_type type_info() const { return type_info_(); }

protected:
    /// Protected default ctor
    FloatBase() = default;

    /// Protected copy ctor
    FloatBase(const FloatBase&) = default;

    /// Protected move ctor
    FloatBase(FloatBase&&) noexcept = default;

    /// Protected copy assignment operator
    FloatBase& operator=(const FloatBase&) = default;

    /// Protected move assignment operator
    FloatBase& operator=(FloatBase&&) noexcept = default;

    /// Dtor
    virtual ~FloatBase() = default;

private:
    /// Derived class implements to return the RTTI information for *this
    virtual rtti_type type_info_() const = 0;
};

} // namespace wtf::fp
