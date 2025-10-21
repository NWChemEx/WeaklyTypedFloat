# WeaklyTypedFloat (WTF)

This repo is still under heavy development! I am hoping for an alpha release
this week.

The goal of WTF is to provide a small domain-specific language (DSL) that can
be used to unify interfaces involving floating-point types. Using WTF the user 
writes their interfaces in terms of "weakly typed" objects like `Float` and 
`FloatBuffer`, and determination of the actual floating-point type is deferred 
to runtime. 

## Problem Description

Full description (TODO: Add link to the documentation)

Depending on the standard, C++ already natively has a lot of floating-point 
types, e.g., `float`, `double`, `long double`, `std::complex<float>`, and
`std::complex<double>`. This is before considering custom floating-point types,
such as those needed for automatic differentiation, or hardware-specific types.
When attempting to maintain interfaces with explicit types (such as is required
for C-style interfaces) this leads to a combinatorial explosion in the number 
of interfaces. 