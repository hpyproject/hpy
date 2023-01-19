HPy Context
===========

The ``HPyContext`` structure is also part of the API since it provides handles
for built-in objects. For a high-level description of the context, please also
read :ref:`api:hpycontext`.

.. warning:: It is fine to use handles from the context (e.g. ``ctx->h_None``)
    but it is **STRONGLY** discouraged to directly call any context function.
    This is because, for example, when compiling for :term:`CPython ABI`, the
    context functions won't be available.

.. autocstruct:: hpy/cpython/autogen_ctx.h::_HPyContext_s
   :members:
