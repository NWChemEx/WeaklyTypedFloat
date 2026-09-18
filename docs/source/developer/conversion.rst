.. Copyright 2026 NWChemEx-Project
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

#################
Conversion in WTF
#################

WTF's type-erased values need to be un-erased before ordinary arithmetic can
be done with them. This page discusses ``wtf::fp::float_cast`` and
``wtf::buffer::contiguous_buffer_cast``, the exact-match tools WTF already has
for that, the friction they cause for callers who want a converted value
instead of an exact one, and ``convert_to``, the policy-driven function that
fills that gap. It also documents ``wtf::cast``, the shared implementation
component ``float_cast``, ``contiguous_buffer_cast``, ``visit_float`` and
friends, and ``convert_to`` are all built on, plus the ``wtf::cast::cast``/
``visit``/``convert`` unified names it offers on top.

.. note::

   Performance portable code should rely on
   ``wtf::buffer::visit_contiguous_buffer``
   to do arithmetic on type-erased values. ``convert_to`` will require deep
   copies of the data and is largely intended for prototyping, testing, and
   other applications which are not performance critical.

**********
Motivation
**********

:doc:`type_erasure` describes how ``wtf::Float`` hides the type of the
floating-point value it holds behind a Holder/Model pair, and how
``float_cast<T>(Float&)`` un-erases it by ``dynamic_cast``-ing the holder to
``FloatModel<std::decay_t<T>>``. ``wtf::buffer::contiguous_buffer_cast<T>``
does the analogous thing for a ``FloatBuffer``, handing back a
``std::span<T>`` that aliases the buffer's contiguous storage.

Both functions require the caller to name the *exact* type that is actually
held; asking for anything else -- even a harmless widening, like getting a
``double`` out of something holding a ``float`` -- throws
``std::runtime_error``. A caller that knows it wants "a ``double``, converting
if necessary" or "a ``double``, but only if no precision would be lost" has no
way to say so today. It would have to hand-roll a fallback chain of
``float_cast<float>``/``float_cast<double>``/... calls itself, and that
fallback chain could not be reused by, or extended for, anyone else.

*****************
Problem Statement
*****************

- ``float_cast``'s and ``contiguous_buffer_cast``'s existing exact-match,
  alias-capable behavior must be left alone. It is backward compatible and it
  is still the right tool for a caller that already knows the held type and
  wants to read or write through it.
- There must be an opt-in "convert if possible" mode, and an opt-in "convert
  only if no precision is lost" mode. Both always produce a new value, since
  neither can alias storage of a type other than the one actually held.
- The set of conversion behaviors must be open to user extension without
  requiring changes to WTF itself, the same way ``WTF_REGISTER_FP_TYPE``
  already lets users add new held types without changing WTF.
- Whatever mechanism selects a behavior must compose with the existing
  ``dynamic_cast``/``FloatModel`` unwrap machinery, since that machinery is
  the only supported way to get from a holder back to a typed value.
- The mechanism must work the same way for a single ``wtf::Float`` and for a
  ``wtf::buffer::FloatBuffer`` of them.

****************
What won't work?
****************

- **A named function per behavior** (``float_cast_convertible``,
  ``float_cast_precision``, ...). This doesn't scale past a couple of
  behaviors, and a user wanting their own behavior would have no function to
  call at all.
- **A runtime enum/flag parameter** selecting among preset behaviors. This
  still needs the same per-candidate-type dispatch under the hood as any
  other approach, and it only ever lets a caller pick among presets WTF
  shipped -- it cannot express a user's own conversion logic.
- **Baking "try to convert" into ``float_cast`` unconditionally.** This breaks
  ``float_cast``'s current exact-match semantics and the tests that depend on
  it, and it would silently allow lossy narrowing conversions no caller asked
  for.
- **Adding a ``Policy`` template parameter directly to ``float_cast``.** This
  was the first shape this design took. The problem: ``float_cast<T&>`` supports
  returning a mutable reference into the held storage (e.g.
  ``float_cast<float&>(f) = 1.23f``), but a converting policy necessarily
  produces a brand new value with no storage to alias. ``float_cast`` would
  end up with two incompatible result contracts depending on which policy was
  selected. Giving the always-by-value behavior its own function name and
  contract, instead of overloading ``float_cast``'s, avoids that.
- **Runtime-polymorphic (virtual) policy objects.** A policy is a
  compile-time strategy selected via a template parameter, not something
  chosen at runtime from a set of live objects; virtual dispatch would add an
  indirection with no corresponding benefit here.
- **A virtual ``accept``/visitor path on ``FloatHolder``**, where a single
  non-templated virtual method takes a fixed visitor interface with one
  ``visit(U&)`` overload per candidate type, so ``FloatModel<T>::accept``
  calls straight into the matching overload with no ``dynamic_cast`` at all.
  This is genuinely faster -- one virtual call instead of a dispatch walk --
  but the visitor interface's overload set has to be closed at the point it
  is declared, since a virtual function cannot itself be templated. That
  reintroduces exactly the closed-set problem :doc:`extensible_enums`
  rejected a plain ``enum class`` for: a type registered downstream via
  ``WTF_REGISTER_FP_TYPE`` after ``FloatHolder`` and the visitor interface
  are already compiled would have no ``visit()`` slot to land in, breaking
  "extend WTF without recompiling it" for this code path.

****************
Current Solution
****************

``convert_to`` is a free function with two overloads:

.. code-block:: c++

   template<typename T, typename Policy = policies::Match,
            typename TupleType = wtf::default_fp_types>
   T wtf::fp::convert_to(FloatView<const Float> f);

   template<typename T, typename Policy = fp::policies::Match,
            typename TupleType = wtf::default_fp_types>
   std::vector<T> wtf::buffer::convert_to(BufferView<const Float> buffer);

``convert_to`` finds the one candidate ``U`` in ``TupleType`` whose model the
view's ``dynamic_cast``-based unwrap actually succeeds against -- the same
``wtf::detail_::dispatch`` fold ``wtf::fp::detail_::visit_float_model`` and
every other multi-candidate WTF dispatch already use (see
:doc:`extensible_enums` and :doc:`operations`) -- then calls
``Policy::convert<T>(value)`` with the restored value. Only ``T`` needs to be
spelled out at the call site -- ``U`` is deduced from the argument. If that
call returns an engaged ``std::optional<T>``, that is the result; otherwise
``convert_to`` throws ``std::runtime_error("wtf::convert_to: bad cast")``,
mirroring ``float_cast``'s error contract. Since ``dynamic_cast`` only ever
matches the one type a holder actually holds, there is never more than one
successful candidate to choose between -- "first successful conversion, in
candidate order, wins" falls directly out of the existing fold, with no
second, convert-specific fold needed.

Taking ``FloatView<const Float>``/``BufferView<const Float>`` by value,
rather than ``Float&``/``FloatBuffer&`` the way ``float_cast`` and
``contiguous_buffer_cast`` do, does two things at once. First, it makes the
"always a new value, never an alias" contract explicit at the signature level
-- a const view cannot be written through regardless of what a policy does.
Second, it means a caller does not need two different overloads depending on
whether they hold a ``Float``/``FloatBuffer`` or a view of one: ``Float``
already implicitly converts to ``FloatView<const Float>`` via
``Float::as_view() const``, and ``FloatBuffer`` implicitly converts to
``BufferView<const Float>`` through ``BufferView``'s converting constructor
(note that, like ``FloatView<X>``, ``BufferView<X>`` is templated on the
*element* type, not on ``FloatBuffer`` itself). The ``BufferView`` overload
applies the ``FloatView`` overload element-wise, via ``BufferView::at``, and
collects the results into a ``std::vector<T>`` -- not a ``std::span<T>``,
since the converted elements are newly created values with no existing
contiguous storage of type ``T`` to span over. This element-wise loop is the
*only* code the ``BufferView`` overload needs: every bit of policy-dispatch
logic lives in the ``FloatView`` overload, called once per element.

A policy is any class exposing:

.. code-block:: c++

   template<typename T, typename U>
   static std::optional<T> convert(U&& value);

Returning ``std::optional<T>`` lets a policy say "not applicable to this
``U``" without having to throw itself, so ``convert_to`` owns the single
"bad cast" error message regardless of which policy is in use. Because
``convert_to`` never returns a reference, every policy's ``convert`` returns
a plain ``T`` by value -- there is no reference-result case to special-case.

WTF ships three built-in policies. They are defined once, in
``wtf::cast::policies`` (see `The wtf::cast component`_ below), and
``wtf::fp::policies`` is simply ``namespace policies = wtf::cast::policies;``,
so they can be spelled either way:

.. code-block:: c++

   auto f = wtf::fp::make_float(3.14f); // holds a float

   // Match: only succeeds when U and T are the same type.
   auto a = wtf::fp::convert_to<float, policies::Match>(f);   // 3.14f
   // wtf::fp::convert_to<double, policies::Match>(f) would throw.

   // Convertible: succeeds whenever U is convertible to T.
   auto b = wtf::fp::convert_to<double, policies::Convertible>(f); // widens

   // Widen: like Convertible, but only for conversions that cannot lose
   // precision.
   auto c = wtf::fp::convert_to<double, policies::Widen>(f);  // OK, widens
   // wtf::fp::convert_to<float, policies::Widen>(make_float(3.14)) would
   // throw, since double -> float can lose precision.

- ``Match`` is engaged only when
  ``std::same_as<std::decay_t<U>, std::decay_t<T>>``. This is exactly what
  ``float_cast<T>`` already checks; ``convert_to`` with ``Match`` differs
  from ``float_cast<T>`` only in returning a copy instead of a possible
  reference.
- ``Convertible`` is engaged when ``std::convertible_to<U, T>`` (checked with
  ``if constexpr``), and returns ``static_cast<T>(value)``.
- ``Widen`` is engaged only when ``type_traits::precision_v<T> >=
  type_traits::precision_v<U>``, reusing the existing
  ``wtf::type_traits::Precision`` trait
  (``cxx/include/wtf/type_traits/precision.hpp``) rather than inventing new
  precision-comparison machinery. This allows converting ``float`` to
  ``double`` but rejects converting ``double`` to ``float``.

The wtf::cast component
========================

``float_cast``, ``contiguous_buffer_cast``, ``visit_float``,
``visit_float_view``, ``visit_contiguous_buffer``,
``visit_contiguous_buffer_view``, and ``convert_to`` used to each hand-roll
their own single-type ``dynamic_cast``-or-throw block, or their own thin
wrapper around ``wtf::detail_::dispatch`` (the latter duplication already
called out by :doc:`kokkos`, which proposed -- but never implemented --
collapsing the two buffer-side wrappers into one ``visit_buffer_as``). All of
that is now factored into a new component, ``wtf::cast``
(``cxx/include/wtf/cast/``), alongside ``fp``, ``buffer``, ``enums``, and
``rtti``:

- ``wtf::cast::detail_::restore<ModelType, T>(holder, msg)`` implements the
  single-type "downcast this holder to ``ModelType<T>``, or throw" pattern.
  ``float_cast``, ``FloatView::value``, ``FloatBuffer::value``, and
  ``BufferView::value`` all call it now, instead of each hand-rolling their
  own ``dynamic_cast``.
- ``wtf::cast::detail_::visit_as<ModelType, TupleType>(visitor, args...)``
  generalizes :doc:`kokkos`'s ``visit_buffer_as`` proposal beyond
  buffer-only: it wraps ``wtf::detail_::dispatch`` and calls the visitor with
  each restored model's ``handle()`` -- a reference to the scalar for the
  ``fp``-side models, a ``std::span`` for the ``buffer``-side ones.
  ``visit_float_model``, ``visit_float_view_model``, ``visit_contiguous_model``,
  and ``visit_contiguous_view_model`` are now one-line forwarders to it.
- ``wtf::cast::detail_::convert_one<T, Policy, ModelType, TupleType>(holder)``
  is what ``wtf::fp::convert_to`` calls: it is ``visit_as`` with a policy
  applied to the restored value.
- ``wtf::cast::policies`` holds ``Match``, ``Convertible``, and ``Widen``.

None of ``float_cast``, ``contiguous_buffer_cast``, ``visit_float`` and
friends, or ``convert_to`` moved -- they all keep their existing names and
namespaces (``wtf::fp``/``wtf::buffer``), since relocating a
friendship-requiring function into a different header would create a
circular ``#include`` between that header and ``Float``'s/``FloatBuffer``'s
own. Only their bodies changed, to call into ``wtf::cast::detail_``.

On top of that shared implementation, ``wtf::cast`` also offers a **unified
dispatch facade**: ``wtf::cast::cast``, ``wtf::cast::visit``, and
``wtf::cast::convert`` are overload sets that call the appropriate
``wtf::fp``/``wtf::buffer`` function based on the argument's type (a plain
``Float``/``FloatView``, or a ``FloatBuffer``/``BufferView``), so a caller
who does not want to remember which domain-specific name to reach for can
just write ``wtf::cast::cast<T>(x)``/``wtf::cast::visit<TupleType>(v, x)``/
``wtf::cast::convert<T>(x)`` regardless of which kind of ``x`` they have.
These overloads add no new logic of their own; they are one-line forwarders,
same as the four ``visit_*_model`` wrappers above.

*****************
Known Limitations
*****************

- ``convert_to`` always allocates a new ``T`` (or ``std::vector<T>``), so it
  is not a viable option  when the caller specifically wants to alias or mutate
  the held storage.
- Policies must be stateless: ``convert_to`` never constructs a ``Policy``
  instance, it only ever calls ``Policy::convert`` as a static member.
- ``TupleType`` must be supplied explicitly to reach a custom-registered
  type, the same caveat ``make_float(FloatKind, ...)`` already has (see
  :doc:`extensible_enums`).

**********************
Further Considerations
**********************

- Two questions raised during design turned out to already be answered by
  existing infrastructure, once the implementation was underway:

  - The error-reporting contract when multiple candidates in ``TupleType``
    could plausibly convert to ``T``: this never actually arises, because
    ``dynamic_cast`` only ever matches the one type a holder truly holds --
    there is no "multiple candidates convertible-but-lossy" case to
    adjudicate between, only "the one true candidate, and whether ``Policy``
    accepts it".
  - Where the shared policy-dispatch logic should live so the ``FloatView``
    and ``BufferView`` overloads can share it without duplicating the fold
    expression: in ``wtf::cast::detail_``, and in practice the ``BufferView``
    overload does not even need its own copy of the dispatch fold -- it
    reuses the ``FloatView`` overload element-wise instead (see
    `The wtf::cast component`_).

- ``handle()`` (the accessor ``visit_as`` calls on every restored model) is
  not a uniform "the buffer" abstraction: it returns a scalar reference for
  the ``fp``-side models and a ``std::span`` for the ``buffer``-side ones.
  This matches each model's own visitors (a ``visit_float`` visitor already
  expects scalars; a ``visit_contiguous_buffer`` visitor already expects
  spans), and mirrors the naming :doc:`kokkos` already committed to for its
  own (not yet implemented) device-view model, but the name reads most
  naturally as "a handle to a buffer" -- worth a second look if it causes
  confusion in practice.
