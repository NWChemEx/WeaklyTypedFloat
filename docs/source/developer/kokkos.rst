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

##########################
Interoperating with Kokkos
##########################

Kokkos provides performance portability for *execution*: one kernel that runs
on CPUs, NVIDIA GPUs, AMD GPUs, and Intel GPUs. WTF provides type erasure for
*data*. This document works out how the two compose, and specifically how WTF
can hand a caller a result that is still resident in device memory without the
caller having to know that.

Per :doc:`scope`, WTF does not implement linear algebra and nothing here
proposes that it should. What is proposed is that WTF be able to *describe* a
buffer a Kokkos kernel produced, so the kernel's caller can stay template-free.

The pleasant surprise of this design is how little of it is new machinery. WTF
already separates "what type is this" from "where does it live" via the
Interface/Holder/Model pattern (:doc:`type_erasure`); device residency turns
out to be just another model at each of the two levels that already exist.

*******************************
Device Memory Belongs to a View
*******************************

``FloatBuffer`` **owns** its memory and ``BufferView`` **aliases** memory owned
by someone else (:doc:`float_buffer`). Today the only owning model,
``ContiguousModel<T>``, stores a ``std::vector<std::decay_t<T>>`` -- a host
allocation, with no allocator or memory-space parameter anywhere in the class.

The temptation is to generalize ``ContiguousModel`` over an allocator so
``FloatBuffer`` can own device memory. We should not, for three reasons.

1. **The host does not own device memory.** ``FloatBuffer``'s contract is that
   it owns what it type-erases and that copying it deep-copies. A device
   allocation is owned by the device runtime; the host holds a handle.
   Promising ownership we do not have is the wrong abstraction.

2. **The owning interface cannot be honestly implemented.** ``BufferHolder``
   requires ``push_back_`` and ``reserve_``. A ``Kokkos::View`` is fixed-size;
   "growing" one means allocate-and-copy, which is a catastrophic operation to
   hide behind an innocuous ``push_back``. A device model deriving from
   ``BufferHolder`` would throw from two of its ten virtuals.
   ``BufferViewHolder`` requires neither, and all eight of its virtuals have a
   sensible device implementation.

3. **The view already says the right thing about copies.**
   ``BufferViewHolder::clone`` is specified as a *shallow* copy: the clone
   still aliases the same buffer. ``Kokkos::View``'s copy constructor is a
   reference-counted shallow copy with exactly that meaning. The model gets
   correct copy semantics by doing nothing.

So: **a device-resident buffer is a** ``BufferView``.

Who Keeps the Allocation Alive?
===============================

This is the one place the device case genuinely differs from the host case.

A host ``BufferView`` aliases memory with an independent owner -- a
``FloatBuffer``, a ``std::vector``, a caller's raw pointer. If that owner dies
first the view dangles; that is the documented bargain of a view.

A device ``BufferView`` returned from a kernel has **no other owner**. The
allocation was made inside the routine; the view is the only handle to it. If
the view does not keep it alive, nothing does.

The resolution is that the *model* holds its ``Kokkos::View`` by value, and
``Kokkos::View`` is reference-counted. Shallow-cloning the model copies the
``Kokkos::View``, incrementing the refcount. When the last WTF handle dies the
refcount hits zero and Kokkos frees the allocation.

So a device ``BufferView`` is, strictly, *participating in* ownership even
though ``BufferView`` is the non-owning class. We consider that acceptable and,
importantly, invisible: the user sees a handle with value semantics that stays
valid as long as they hold it. The alternative -- making the user keep a
separate owner object alive alongside the view -- leaks the device into the
API, which is exactly what we are avoiding.

.. note::

   Worth restating in the class docs if we adopt this: ``BufferView`` does not
   own its memory, *except* that a device-backed view shares ownership of the
   device allocation with every handle cloned from it.

*********************
The Device View Model
*********************

.. code-block:: c++

   template<concepts::FloatingPoint FloatType, typename MemorySpace>
   class DeviceViewModel
     : public BufferViewHolder<apply_const_t<fp::Float, FloatType>> {
       // ...
       Kokkos::View<FloatType*, MemorySpace> m_view_;
   };

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Virtual
     - Implementation
   * - ``size_()``
     - ``m_view_.extent(0)``
   * - ``is_contiguous_()``
     - ``true`` -- a rank-1 ``View`` with the default layout is contiguous
   * - ``is_const_()``
     - ``std::is_const_v<FloatType>``
   * - ``clone_()`` / ``const_clone_()``
     - Copy ``m_view_``; the intended shallow, refcounted copy
   * - ``at_(i)``
     - Returns a device-backed ``FloatView`` -- see below
   * - ``are_equal_(other)``
     - ``Kokkos::parallel_reduce`` over the two ``View`` objects

There is no ``push_back_`` or ``reserve_`` to implement. That is the whole
argument for putting this on the view side.

**********************************
Element Access: a Device FloatView
**********************************

``at_`` looks like the sharp edge. It returns an ``fp::FloatView``, and the
existing ``ContiguousViewModel`` builds one from a *reference* to an element
(``return view_type(m_buffer_[index]);``). Forming a host reference to
``CudaSpace`` memory is a segfault, not an exception.

But ``FloatView`` is itself a PIMPL over ``FloatViewHolder``, and it has a
public constructor taking a ``holder_pointer``. So ``at_`` does not have to
produce a reference -- it can produce a *model*. The question becomes whether
``FloatViewHolder``'s interface is implementable over device memory.

It is. Its pure virtuals are ``is_const_``, ``clone_``, ``const_clone_``,
``change_value_``, ``are_equal_``, and ``to_string_``. **None of them returns a
reference to the value.** The reference-returning ``get_value()``/``data()``
live on the concrete ``FloatViewModel``, not on the type-erased interface. So
we add a sibling model:

.. code-block:: c++

   template<concepts::FloatingPoint FloatType, typename MemorySpace>
   class DeviceFloatViewModel : public FloatViewHolder<...> {
       // rank-0 View aliasing exactly one element
       Kokkos::View<FloatType, MemorySpace> m_value_;
   };

and ``DeviceViewModel::at_`` becomes:

.. code-block:: c++

   view_type at_(size_type i) override {
       using model = DeviceFloatViewModel<FloatType, MemorySpace>;
       return view_type(std::make_unique<model>(Kokkos::subview(m_view_, i)));
   }

``Kokkos::subview(m_view_, i)`` yields a rank-0 ``View`` aliasing element ``i``
*without dereferencing anything on the host*, and it refcount-shares the parent
allocation. The lifetime story therefore holds all the way down to a single
element: a ``FloatView`` into a device buffer keeps that buffer alive, exactly
as a host ``FloatView`` into a ``FloatBuffer`` would be expected to.

The remaining virtuals are all scalar transfers:

- ``change_value_(other)`` -- ``Kokkos::deep_copy`` from a host scalar.
- ``are_equal_(other)`` -- ``deep_copy`` both to host, compare.
- ``to_string_()`` -- ``deep_copy`` to host, format.

Reading or writing one element of a device buffer is thus **correct but slow**:
it costs a 4- or 8-byte transfer and a fence. That is the right failure mode
for a convenience accessor. The user who writes ``d_P.at(7)`` in a loop gets
bad performance; the user who writes it once in a debug print gets the right
answer. Neither gets undefined behavior.

What Still Has to Be Gated
==========================

With ``at()`` handled, the only remaining hazards are the APIs that hand out a
raw host pointer and cannot be made safe: ``BufferView::value<T>()``,
``contiguous_buffer_view_cast``, and the models' ``data()``. Those need a
runtime query, sibling to the existing ``is_contiguous()``:

.. code-block:: c++

   /// Can the elements of this buffer be reached by a host pointer?
   bool is_host_accessible() const;

backed by a new ``virtual bool is_host_accessible_() const`` defaulted to
``true`` in ``BufferViewHolder`` and ``BufferHolder``, so ``ContiguousModel``
and ``ContiguousViewModel`` are untouched. ``DeviceViewModel`` returns
``Kokkos::SpaceAccessibility<Kokkos::HostSpace, MemorySpace>::accessible`` --
``true`` for ``HostSpace`` and ``CudaUVMSpace``, ``false`` for ``CudaSpace``.
The pointer-returning methods throw "not host accessible; call ``to_host()``"
rather than faulting.

Note this is a narrower gate than it first appeared. ``is_host_accessible()``
answers "may I do pointer arithmetic here", which is precisely the question
``is_contiguous()`` already answers for layout. The two belong together.

********************************
Dispatching to a Portable Kernel
********************************

WTF already has the right hook.
``visit_contiguous_buffer_view<TupleType>`` resolves each argument's type once,
with a single ``std::visit`` over ``dynamic_cast`` results, and hands the
visitor ``std::span<T>...``. Crucially, **the type-erasure cost is paid once
per call, at the buffer level -- never per element**. That is what makes this
composable with a kernel at all.

Rather than write a device-specific copy of that function, note that the two
existing visit helpers are already near-duplicates:

.. code-block:: c++

   // contiguous_model.hpp
   auto lambda = [&](auto&&... a) { return visitor(a.span()...); };
   return wtf::detail_::dispatch<ContiguousModel, TupleType>(lambda, ...);

   // contiguous_view_model.hpp -- identical but for the model
   auto lambda = [&](auto&&... a) { return visitor(a.span()...); };
   return wtf::detail_::dispatch<ContiguousViewModel, TupleType>(lambda, ...);

Both bodies vary in exactly one token. So hoist the model and give every model
a uniformly-named accessor for "the thing you iterate":

.. code-block:: c++

   template<template<concepts::FloatingPoint> typename ModelType,
            typename TupleType, typename Visitor, typename... Args>
   auto visit_buffer_as(Visitor&& visitor, Args&&... args) {
       auto lambda = [&](auto&&... a) { return visitor(a.handle()...); };
       return wtf::detail_::dispatch<ModelType, TupleType>(
         lambda, std::forward<Args>(args)...);
   }

where ``handle()`` returns ``std::span<T>`` on the contiguous models and
``Kokkos::View<T*, Space>`` on the device model. Every visit helper then
collapses to a one-line forwarder, and **the device model needs no new dispatch
code at all**. These are all ``detail_`` classes, so renaming ``span()`` to
``handle()`` is not a public API break; ``span()`` can stay as an alias on the
contiguous models where the name is honest.

This is a refactor worth doing regardless of Kokkos -- it deletes duplication
that is in the tree today.

One Wrinkle
===========

``dispatch``'s model parameter is declared
``template<concepts::FloatingPoint> typename ModelType`` -- exactly one
argument -- and ``DeviceViewModel<T, Space>`` takes two. Bind the space with an
alias template, which is a valid template-template argument:

.. code-block:: c++

   template<concepts::FloatingPoint T>
   using DefaultDeviceViewModel =
     DeviceViewModel<T, Kokkos::DefaultExecutionSpace::memory_space>;

A given call therefore dispatches over one memory space at a time. That seems
right: mixing spaces within one kernel launch is not meaningful anyway.

Two Constraints on the Visitor
==============================

``wtf::default_fp_types`` is ``std::tuple<float, double, long double>``, and
``long double`` has no device support. Every device-facing visit must pass a
restricted tuple, e.g. ``std::tuple<float, double>``. A buffer holding
``long double`` then throws from dispatch, which is the correct outcome.

More subtly: **the visitor must be a named functor, not a generic lambda.**
``nvcc`` restricts where an extended ``__device__`` lambda may be defined, and
inside a generic lambda is one of the restricted contexts. Since WTF delivers
the recovered type *through* a generic lambda, a ``KOKKOS_LAMBDA`` written
directly in the visitor body is a portability hazard. The pattern that works: a
struct with a templated ``operator()`` forwarding to a free function template
that contains the ``KOKKOS_LAMBDA``. WTF's own tests already use named-functor
visitors, so this matches existing practice.

*******
Example
*******

Rotate ``n`` points by a 3x3 rotation matrix. Inputs and output are type-erased
and may be device-resident; the caller writes no templates and names no memory
space.

The kernel -- a free function template, so ``KOKKOS_LAMBDA`` sits in a context
``nvcc`` accepts:

.. code-block:: c++

   template<typename Scalar, typename Space>
   Kokkos::View<Scalar*, Space> rotate_points_kernel(
     Kokkos::View<const Scalar*, Space> R,
     Kokkos::View<const Scalar*, Space> P) {
       using exec = typename Space::execution_space;
       const std::size_t n = P.extent(0) / 3;

       Kokkos::View<Scalar*, Space> Q(
         Kokkos::view_alloc("Q", Kokkos::WithoutInitializing), 3 * n);

       Kokkos::parallel_for(
         "wtf::rotate_points", Kokkos::RangePolicy<exec>(0, n),
         KOKKOS_LAMBDA(const std::size_t i) {
             const Scalar x = P(3 * i + 0);
             const Scalar y = P(3 * i + 1);
             const Scalar z = P(3 * i + 2);
             for(int j = 0; j < 3; ++j) {
                 Q(3 * i + j) = R(3 * j + 0) * x + R(3 * j + 1) * y +
                                R(3 * j + 2) * z;
             }
         });
       return Q;
   }

.. note::

   All ``n`` threads read the same 9 elements of ``R``. A production kernel
   would stage those in scratch or registers; it is left explicit here so the
   indexing is readable.

The visitor -- templated ``operator()``, no lambda:

.. code-block:: c++

   struct RotateVisitor {
       wtf::buffer::BufferView<wtf::fp::Float>* m_prv;

       template<typename R, typename P, typename Space>
       void operator()(Kokkos::View<R*, Space> r,
                       Kokkos::View<P*, Space> p) const {
           using r_t = std::remove_const_t<R>;
           using p_t = std::remove_const_t<P>;
           if constexpr(!std::is_same_v<r_t, p_t>) {
               throw std::runtime_error(
                 "rotate_points: rotation and points must hold the same type");
           } else {
               if(r.extent(0) != 9)
                   throw std::runtime_error("Rotation must have 9 elements");
               if(p.extent(0) % 3)
                   throw std::runtime_error("Points must be a multiple of 3");

               *m_prv = wtf::buffer::make_device_view(
                 rotate_points_kernel<r_t, Space>(r, p));
           }
       }
   };

.. note::

   Dispatch is a full Cartesian product over the type list, so the mixed-type
   instantiations are reachable and must compile. Rejecting them at runtime is
   the simple option; promoting to ``std::common_type_t`` on the device is
   possible but costs a conversion pass, so we defer that until something needs
   it.

The public API -- template-free, and it never names a memory space:

.. code-block:: c++

   wtf::buffer::BufferView<wtf::fp::Float> rotate_points(
     wtf::buffer::BufferView<const wtf::fp::Float> R,
     wtf::buffer::BufferView<const wtf::fp::Float> points) {
       using device_types = std::tuple<float, double>;
       wtf::buffer::BufferView<wtf::fp::Float> rv;
       wtf::buffer::visit_buffer_as<DefaultDeviceViewModel, device_types>(
         RotateVisitor{&rv}, R, points);
       return rv;
   }

And the call site:

.. code-block:: c++

   wtf::buffer::FloatBuffer h_R{/* 9 elements */};
   wtf::buffer::FloatBuffer h_P{/* 3n elements */};

   auto d_R = wtf::buffer::to_device(h_R);   // host -> device view
   auto d_P = wtf::buffer::to_device(h_P);

   auto d_Q  = rotate_points(d_R, d_P);      // result stays on device
   auto d_Q2 = rotate_points(d_R, d_Q);      // ... and feeds the next call

   std::cout << d_Q2.at(0) << "\n";          // works: one scalar transfer
   auto h_Q = wtf::buffer::to_host(d_Q2);    // FloatBuffer, only when needed

``d_Q`` is a ``BufferView``. It is the only handle to its device allocation and
keeps it alive exactly as long as it lives. Nothing at the call site names
``Kokkos``, a ``Scalar``, or a memory space.

The chained call is the payoff: because the intermediate never returns to the
host, composing two device routines costs no transfers. And the ``at(0)`` on
the next line still works -- it just pays for what it does.

Supporting Free Functions
=========================

- ``make_device_view(Kokkos::View<T*, Space>)`` -- wraps a ``View`` in a
  ``BufferView``. What a kernel author calls to re-erase a result.
- ``to_device(const FloatBuffer&)`` / ``to_device<Space>(...)`` -- allocates in
  the target space and ``deep_copy``s. Returns a ``BufferView``.
- ``to_host(BufferView)`` -- returns a ``FloatBuffer``, since the host really
  does own that one.

**************
Open Questions
**************

- Should ``to_device`` on an already-device view be the identity or an explicit
  copy? Identity is cheaper; explicit is less surprising.
- ``are_equal_`` via ``parallel_reduce`` means ``operator==`` launches a
  kernel. Acceptable for a comparison operator, or should comparing a
  non-host-accessible view force an explicit ``to_host``?
- Scalar ``at()`` access fences the execution space. Should a device
  ``FloatView`` cache its value on first read, and if so, how does it learn
  that the underlying buffer changed?
- Does ``BufferView``'s implicit conversion from ``FloatBuffer`` need
  suppressing for device models, or is it naturally unreachable?
- Where does this live? A ``WTF_ENABLE_KOKKOS`` component keeps WTF's core free
  of a hard Kokkos dependency but splits buffer behavior across a build flag. A
  satellite repo avoids that but leaves the ``is_host_accessible()`` hook
  (which must live in core) looking orphaned.
- WTF declares ``cmake_minimum_required(VERSION 3.14)``; Kokkos 4.x needs
  3.16+.
