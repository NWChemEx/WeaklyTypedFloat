These are notes on how to register a new type with WTF.

.. note::

   The macro ``WTF_REGISTER_TYPE`` will do all of this for you.

- Specialize IsFloatingPoint for your type
- Specialize TypeName for your type

Optional steps:

- Specialize Precision
- Ensure implicit conversions are visible to/from your type.