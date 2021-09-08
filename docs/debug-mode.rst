Debug Mode
==========

HPy includes a debug mode which includes a lot of useful run-time checks to
ensure that C extensions use the API correctly. The major points of the debug mode are:

    1. no special compilation flags are required: it is enough to compile the extension 
       with the Universal ABI.
    
    2. The debug mode can be activated at *import time*, and it can be activated
       per-extension.
    
    3. You pay the overhead of the debug mode only if you use it. Extensions loaded 
       without the debug mode run at full speed.

This is possible because the whole of the HPy API is provided
as part of the HPy context, so debug mode can pass in a special debugging
context (that wraps the normal context) without affecting the performance of
the regular context at all.

The debugging context can already check for:

* Leaked handles.
* Handles used after they are closed.

An HPy module may be loaded in debug mode using::

  mod = hpy.universal.load(module_name, so_filename, debug=True)
