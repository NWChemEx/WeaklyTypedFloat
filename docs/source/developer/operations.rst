.. Copyright 2025 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

###############################
Performing Type-Safe Operations
###############################

A key challenge in the design of WTF is how to perform arbitrary numeric
operations in a performant and type-safe manner.

***************
Single Dispatch
***************

C++ natively supports single dispatch through virtual functions. This means that
something like:

.. code-block:: c++

   struct MyFunctor {
         template<typename T>
         void run(T val) {
              // do something with val
         }
    };

   wtf::Buffer b = fxn_that_returns_a_buffer_of_wtf_floats();
   b.evaluate(MyFunctor{});

can be implemented by having the ``evaluate`` function call a virtual function
on the holder. The model class, then implements ``evaluate`` by calling the
``run`` method on the functor it was given and passing it the type-restored
value. This would look something like:

.. code-block:: c++

   // Class that can wrap any functor with a run method (assumes a set of types)
   struct FunctorInterface;

   BufferHolder::evaluate(FunctorInterface& f) {
        // call the virtual function on the holder
        m_holder->evaluate(f);
   }

   // Drived from BufferHolder, has the value, i.e., `m_value_` of type T
   BufferModel<T>::evaluate(FunctorInterface& f) {
        // call the run method on the functor, passing the type-restored value
        f.run(m_value_);
   }


This could relatively easily be extended to forward any result back to
the user by capturing the return value in a lambda and then binding the lambda
to the functor.

***************
Double Dispatch
***************

The tricky part here is that to call the ``run`` method we must be inside an
object that knows not only the type of the value it is holding, but also the
type of the value being passed in.

.. code-block:: c++

   struct MyFunctor {
        template<typename T, typename U>
         void run()(T val0, U val1) {
              // do something with val0 and val1
         }
    };

The polymoprhic solution we used for single dispatch only works on one type at
a time (the type of ``*this``), so we need a different solution.

What won't work?
================

At this point note that though ``std::variant`` is a common C++ solution for
multiple dispatch. Instead of using the standard type-erasure pattern we'd
hold the value in ``std::variant`` (the ``std::variant`` would live in the
the class that is currently the interface or the class that is the holder and
the model class would no longer be needed). However, this solution only works if
we know ALL of the types ahead of time. Alternatively, we could template the
interface on the ``std::variant`` (or the types in the ``std::variant``) to
avoid assuming a set of types, but this then destroys the type-erasure WTF is
trying to implement since the interface is now templated!

Another common solution is the visitor pattern, but ultimately that
will require a class like:

.. code-block:: c++

    struct Visitor {
        virtual void visit(float, float) = 0;
        virtual void visit(float, double) = 0;
        virtual void visit(float, long double) = 0;
        virtual void visit(double, float) = 0;
        virtual void visit(double, double) = 0;
        virtual void visit(double, long double) = 0;
        virtual void visit(long double, float) = 0;
        virtual void visit(long double, double) = 0;
        virtual void visit(long double, long double) = 0;
    };

    // With some template trickery we could then get the following to work:
    Visitor& v = ...; // User-defined derived class passed by base
    wtf::Buffer val0;
    wtf::Buffer val1;
    dispatch(v, val0, val1);

(n.b., for double dispatch we can make the visitor's interface linear in the
number of FP types by relying on polymorphism to work out one of the types;
however, it will still result in a combinatorial explosion for dispatching on
three or more inputs).  This suffers from the same problem as ``std::variant``:
we don't want to assume a list of FP types (in fact, as the name ``std::visit``
suggest, the two are related...).


FWIW, there's a name for wanting to have multiple dispatch while not wanting to
assume a list of types: it's called the "expression  problem".

Solving the Expression Problem
==============================

At present, solving the expression problem requires a mix of storing RTTI
(runtime type information) and brute force looping over class hierarchies.
Making sure that all the edge cases are handled correctly is tricky and tedious.
Note that this all needs to be done with template meta-programming, so it's also
hideous to look at. So let's see if there is an existing solution that avoids
us needing to reinvent the wheel.

One of the first solutions we found was the YOMM2 library (I'm guessing it
stands for yorel open multi-method; not sure what yorel means...). Yomm2
provides a C++17 solution to  the expression problem. Based on preliminary
investigations it suffers from weird scoping rules. More specifically, if you
define everything in one file it works, but if you try to split it up into
multiple files you can break it. Given that it's all native C++, the scope rules
that Yomm2 ultimately follows are that of C++ itself. However, Yomm2 is a
complicated web of C macros and template meta-programming making it quite
difficult to ascertain exactly what part of the C++ language any given Yomm2
construct is. While this could eventually be worked through, we also noted that
the author of Yomm2 appears to be in the process of moving Yomm2 to Boost. If
we want to consider Yomm2, we should probably wait until it's in Boost, and then
use the Boost version.

****************
Current Solution
****************

For now our solution is to assume that the user of WTF is only supporting a
finite set of floating point types and that they know what those types are. If
that is the case, the user can provide us with the list of types they support
when they want to dispatch. WTF can then use that list to create a series of
``std::variant`` objects for the operation, use ``std::visit`` to do the
multiple dispatch, and then return the result. By comparison, the usual
``std::variant`` solution forces the same set of types on all type-erased
objects in the hierarchy and on all operations using those types. Our solution
still allows the objects to erase arbitrary floating point types, but now
restricts each execution of an operation to a set of types. Of note, each time
an operation is invoked it can be invoked with a different set of types. Of
course, if the held floating-point type is not convertible to one of the types
in the set, an exception will be thrown at runtime.

Past Implementations
=====================

We tried recursion, but...

Our first implementation of the "downcast to a ``std::variant`` alternative"
step used a recursive template ``downcast_impl<I, VariantType>``: it tried
``dynamic_cast`` to the ``I``-th alternative of ``VariantType``, and if that
failed it called itself with ``I + 1``, terminating (by throwing) once ``I``
ran off the end of the variant. Multi-argument dispatch was built the same
way: a ``DispatchHelper<VariantTuple>`` class accumulated one resolved
``std::variant`` per argument, and a pair of mutually recursive functions
(``dispatch_impl``/``dispatch_impl_common``) downcast one holder per
recursion step, each step producing a *new* ``DispatchHelper<...>``
instantiation (via ``std::tuple_cat``) to hold the growing tuple of variants.

Both recursions worked, but neither actually needed to be a recursion: the
loop over a variant's alternatives, and the downcasting of each dispatch
argument, are independent per-step, not a chain of steps that depend on each
other's results. Implementing them as recursive template instantiation
anyway meant the compiler instantiated one distinct template (and one
distinct ``DispatchHelper<...>`` type) per index/argument, so error messages
and compile times got visibly worse the more candidate types or dispatch
arguments were involved, without buying anything in return.

Current Implementation
=======================

Both recursions have been replaced with non-recursive, fold-expression-based
equivalents that produce the same ``std::variant`` alternatives and the same
combinatorial ``std::visit`` call, just without a deep chain of template
instantiations.

``downcast_impl`` now expands a ``std::index_sequence`` over the variant's
alternatives and tries each one in a single fold expression, short-circuiting
on the first ``dynamic_cast`` that succeeds (tracked with a
``std::optional<VariantType>``) and throwing only if none of them match.

For multi-argument dispatch, each holder's downcast is independent of every
other holder's, so ``dispatch`` no longer threads an accumulator through a
recursive call chain. Instead, each argument is downcast on its own (via a
per-argument helper, ``downcast_arg``, which also flattens the "was the
holder const?" branch into a single variant type using a small
``variant_cat_t`` helper), the resulting variants are collected into one
``std::tuple`` by ordinary parameter-pack expansion, and exactly one
``std::visit`` is made across all of them at the end. The braced-init-list
used to build that tuple preserves left-to-right evaluation order, so a
holder whose type isn't in the candidate list still throws deterministically
from the leftmost offending argument, matching the old recursive behavior.
