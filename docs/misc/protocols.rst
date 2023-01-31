###################################
Design notes: Specialized Protocols
###################################

`Note: these are only design notes. The API is not finalized, nor implemented yet.`

Protocols can help remove abstraction overhead when possible. For example,
consider the case of iterating over a sequence (list, tuple, array.array, etc.)
that happens to contain integers.

**Design goals**

* Expose internal data representations, which might be more efficient than
  objects.

* Make the direct access to native data structures explicit to allow
  implementors to adapt without being bound to internal details.

* Keep the API similar for both simple object access and optimised data
  structures.

Considering the iteration protocol, which would look as follows:

.. literalinclude:: protocols-code.c
    :start-after: // BEGIN: foo
    :end-before: // END: foo


Optimised variant when a sequence of C long integers is expected:

.. literalinclude:: protocols-code.c
    :start-after: // BEGIN: bar
    :end-before: // END: bar
