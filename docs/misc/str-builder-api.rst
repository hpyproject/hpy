bytes/str building API
=======================

We need to design an HPy API to build ``bytes`` and ``str`` objects. Before making
any proposal, it is useful to understand:

1. What is the current API to build strings.

2. What are the constraints for alternative implementations and the problems
   of the current C API.

3. What are the patterns used to build string objects in the existing
   extensions.

Some terminology:

- "string" indicates either ``bytes`` or ``str`` objects

- "unicode" or "unicode string" indicates ``str``

A note about naming: in this issue I am translating ``PyUnicode_*`` into
``HPyStr_*``. See issue #xxx for more discussion about the naming convention.
