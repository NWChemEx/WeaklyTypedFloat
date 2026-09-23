###################
Potential Solutions
###################

.. note::

   This is meant to be a page detailing other solutions aside from WTF and
   needs to be fleshed out more.

A fundamental problem in the software engineering of scientific libraries is
dealing with FP types. Historically, for simplicity many scientific libraries
have assumed ``double`` as the FP type at all interfaces. If the user is
storing their data as ``float``, they must convert it to ``double`` to call the
interface, and then convert the result back to ``float`` after the call.
Admittedly, this is why many scientific libraries provide overloads for other
FP types, but this in turn requires the developer to maintain one interface
per FP type.

C++ libraries can avoid the need to support multiple overloads by using
templates. As long as the user of the library is calling the function from C++,
this solution works well. However, it is becoming increasingly important to
be able to interface scientific software to other languages (e.g., Python). In
most cases, interfacing is done through a C-like interface, precluding the use
of templates.