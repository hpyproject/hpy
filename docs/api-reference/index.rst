API Reference
============= 

HPy's public API consists of three parts: 

1. The **Core API** as defined in file ``public_api.h`` 
2. **HPy Helper** functions 
3. **Inline Helper** functions

.. warning:: Generated API reference documentation is work in progress. Some
    parts of the API are not included in this documentation yet.


Core API
--------

The **Core API** consists of inline functions that immediately call into the
Python interpreter.

.. toctree::
   :maxdepth: 2

   hpy-type
   hpy-field
   hpy-global
   hpy-gil
   argument-parsing
   builder


HPy Helper Functions
--------------------

**HPy Helper** functions are functions (written in C) that will be compiled
together with the HPy extension's sources. The appropriate source files are
automatically added to the extension sources. The helper functions will, of
course, use the core API to interact with the interpreter. The main reason for
having the helper functions in the HPy extension is to avoid compatiblilty
problems due to different compilers.

.. toctree::
   :maxdepth: 2

   argument-parsing
   build-value
   helpers


Inline Helper Functions
-----------------------

**Inline Helper** functions are ``static inline`` functions (written in C).
Those functions are usually small convenience functions that everyone could
write but in order to avoid duplicated effort, they are defined by HPy.

.. autocmodule:: hpy/inline_helpers.h
   :members:
