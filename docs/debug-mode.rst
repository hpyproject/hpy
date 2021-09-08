Debug Mode
==========

HPy includes a debug mode that may be activated at *run time* with
*no overhead*. This is possible because the whole of the HPy API is provided
as part of the HPy context, so debug mode can pass in a special debugging
context (that wraps the normal context) without affecting the performance of
the regular context at all.

The debugging context can already check for:

* Leaked handles.
* Handles used after they are closed.

An HPy module may be loaded in debug mode using::

  mod = hpy.universal.load(module_name, so_filename, debug=True)
