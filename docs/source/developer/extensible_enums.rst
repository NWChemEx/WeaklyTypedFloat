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

###############################
Extensible Runtime Enumerators
###############################

WTF needs a way for callers to select a floating-point type at *runtime*
(not just at compile time, via a template parameter). This page motivates
that need and explains the design of ``wtf::enums::FloatKind``, the
extensible, enum-like value WTF uses for it.

**********
Motivation
**********

Runtime factories like ``make_float`` and ``FloatBuffer``'s constructors
need to pick an underlying FP type from something other than a compile-time
template parameter. A common case is that the desired type crossed a
language boundary (e.g. from Python, or from a config file) as a string, so
there is no ``T`` available for the compiler to deduce or for the caller to
spell out explicitly. Historically, this kind of runtime type selection has
been done with a plain ``enum class``.

The problem is that a plain ``enum class`` is a closed set: its enumerators
are fixed the moment WTF itself is compiled. :doc:`statement_of_need`
lists, as one of WTF's goals, that "the user can extend WTF to support their
own custom FP types without needing to modify the WTF source" -- and indeed,
:doc:`adding_a_new_type` and ``WTF_REGISTER_FP_TYPE`` already let a
downstream consumer do exactly that for compile-time use. TensorWrapper, for
example, registers 10 custom scalar types this way. But none of those custom
types could ever appear in a plain enum shipped by WTF, because adding an
enumerator means patching and recompiling WTF.

*****************
Problem Statement
*****************

We need two things:

- A small, comparable, copyable value that identifies "which registered FP
  type is this". It should be seeded with an enumerator for each type in
  ``wtf::default_fp_types`` (``float``, ``double``, ``long double``), and it
  must be possible to add more enumerators after WTF has already been
  compiled.
- A way to convert that value to and from a human-readable string, since a
  string is the form this value usually takes at a language/format boundary
  (e.g. a config file, a Python argument, an error message).

Critically, extending the set of enumerators must be a side effect of the
registration step that already exists (``WTF_REGISTER_FP_TYPE``). Asking
users to perform a *second*, separate registration call for the runtime
enum would be exactly the kind of extra maintenance burden WTF is trying to
spare its adopters.

*****************
What won't work?
*****************

- **A plain ``enum class FloatKind { Float, Double, LongDouble };``.** This
  is the closed-set problem described above, and the reason we're not just
  doing the obvious thing.
- **Skip the enum, dispatch on ``std::string`` everywhere.** This works, but
  throws away everything an enum-like value is good for: cheap, O(1)
  equality/hashing without repeated string comparisons; cheap storage as a
  map key or in a tight loop; and a natural switch/lookup-table shape for
  factories. We still want the string as the *serialization* form of the
  value, not its primary, everyday form.
- **Wrap ``wtf::rtti::TypeInfo``/``TypeHolder``.** WTF already has a
  runtime, name-keyed registry of types in the ``rtti`` module, so reusing
  it is tempting. But ``TypeHolder``'s registry supports only "do I already
  have a model for this name" -- it has no enumeration or id-based reverse
  lookup, and there's no reason to add one, since ``TypeHolder`` is bundled
  with concerns that have nothing to do with an enum: const-qualification,
  whether the type is ``std::nullptr_t``, and tuple-based implicit
  convertibility (see :doc:`type_erasure`). Extending ``TypeHolder``'s
  public surface just to serve this feature would leak an unrelated concern
  into the RTTI module.
- **Build a fully generic ``ExtensibleEnum<Tag>`` template up front.** We
  only have one concrete need today (floating-point types). Generalizing now
  would mean guessing at an API shape with nothing to validate it against.
  We'd rather ship the concrete ``FloatKind`` and generalize later, if and
  when a second use case actually shows up.

****************
Current Solution
****************

``wtf::enums::FloatKind`` is a small value type that wraps an integer id.
Ids are assigned by ``wtf::enums::detail_::FloatKindRegistry``, a Meyer's
singleton holding a name-to-id map (and its inverse). The registry is seeded
at construction with the names of ``wtf::default_fp_types``, so
``from_string("double")`` works even if nothing has wrapped a ``double``
yet. Beyond the seeded defaults, new names are registered lazily and
idempotently: registering the same name twice just returns the id it was
already assigned.

``FloatKind`` objects can only be created through two entry points, mirroring
the private-constructor-plus-friend-factory pattern ``wtf::rtti::TypeInfo``
already uses for ``wtf_typeid<T>()``:

- ``wtf::enums::float_kind<T>()`` -- registers (if necessary) and returns
  the ``FloatKind`` for a compile-time type ``T``.
- ``wtf::enums::from_string(name)`` -- looks up the ``FloatKind`` registered
  under ``name``, throwing ``std::runtime_error`` if nothing is registered
  under that name.

``wtf::enums::to_string(kind)`` converts the other direction.

``WTF_REGISTER_FP_TYPE(T)`` (see ``wtf/type_traits/type_traits.hpp``) was
extended to also emit a static-initializer that calls
``wtf::enums::float_kind<T>()`` once. This means registering a type for use
with WTF at all -- the thing users already had to do -- is the only thing
required to make it show up in ``from_string`` too. There is no second
registration step.

Runtime type selection is then just a matter of walking a caller-supplied
candidate tuple looking for the type whose ``float_kind<T>()`` matches a
given ``FloatKind``. ``wtf::enums::detail_::dispatch_by_kind<TupleType>``
implements this as a non-recursive fold expression over the tuple's indices
-- the same non-recursive, index-sequence-driven style
:doc:`operations`'s "Current Implementation" section adopted for multi-type
dispatch, and for the same reason: a recursive template here would
instantiate one distinct function per candidate type for no benefit, since
each candidate is checked independently of the others.

``make_float`` and ``make_float_buffer`` each gained an overload that takes
a ``FloatKind`` instead of a template parameter, built directly on
``dispatch_by_kind``:

.. code-block:: c++

   auto kind = wtf::enums::from_string("double"); // recovered at runtime

   auto f = wtf::fp::make_float(kind, 3.14);
   auto b = wtf::buffer::make_float_buffer(kind, /*n=*/10);

Both default their candidate ``TupleType`` to ``wtf::default_fp_types``, so
callers only need to name a candidate tuple explicitly when they want a
custom type to be selectable.

*****************
Known Limitations
*****************

- **Static-init registration can be dropped by the linker.** The
  auto-registration in ``WTF_REGISTER_FP_TYPE`` is a static-init side
  effect with no explicit call site. If the translation unit invoking the
  macro is linked into a static library and nothing else in that TU is
  referenced by the final binary, the linker is free to drop the whole
  object file -- and the registration along with it. If a registered type
  mysteriously fails to show up in ``from_string``, this is the first thing
  to check; the workaround is to call ``wtf::enums::float_kind<T>()``
  explicitly once at startup.
- **A new, narrow dependency from ``type_traits`` on ``enums``.** Housing
  the auto-registration inside ``WTF_REGISTER_FP_TYPE`` means
  ``wtf/type_traits/type_traits.hpp`` now includes
  ``wtf/enums/detail_/kind_dispatch.hpp``. This is a deliberate, narrow
  exception to keeping ``type_traits`` a pure leaf module: it is isolated to
  one small header, rather than pulling in the whole ``enums`` public API,
  specifically so this dependency stays easy to point to and doesn't grow
  by accident.
- **``FloatKind::operator<`` reflects registration order, not precision.**
  It exists so ``FloatKind`` objects can be used in ordered containers; it
  is not a proxy for the numeric properties of the underlying type (for
  that, see ``wtf::type_traits::Precision``).

**********************
Further Considerations
**********************

- If a second extensible enum is ever needed elsewhere in WTF, the natural
  refactor is to lift ``FloatKindRegistry``'s logic into a
  ``template<typename Tag> class ExtensibleEnumRegistry`` and make
  ``FloatKind`` a thin instantiation of a corresponding
  ``ExtensibleEnum<Tag>``. We deliberately did not build that generalization
  now (see "What won't work?" above).
- ``make_float(FloatKind, Source)`` currently requires ``Source`` to be
  ``static_cast``-convertible to every type in the candidate ``TupleType``.
  This is sufficient for every type WTF and its known downstream consumers
  register today, but a type that needs a different construction path (e.g.
  one that isn't constructible from a single arithmetic value) would need a
  new overload.
