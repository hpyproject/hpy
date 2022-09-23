Porting Example
===============

HPy supports *incrementally* porting an existing C extension from the
original Python C API to the HPy API and to have the extension compile and
run at each step along the way.

Here we walk through porting a small C extension that implements a Point type
with some simple methods (a norm and a dot product). The Point type is minimal,
but does contain additional C attributes (the x and y values of the point)
and an attribute (obj) that contains a Python object (that we will need to
convert from a `PyObject *` to an `HPyField`).

.. toctree::
   :hidden:
   :glob:

   steps/*


There is a separate C file illustrating each step of the incremental port:

* :doc:`steps/step_00_c_api`: The original C API version that we are going to
  port.

* :doc:`steps/step_01_hpy_legacy`: A possible first step where all methods still
  receive `PyObject *` arguments and may still cast them to `PyPointObject *`
  if they are instances of Point.

* :doc:`steps/step_02_hpy_legacy`: Shows how to transition some methods to HPy
  methods that receive `HPy` handles as arguments while still supporting legacy
  methods that receive `PyObject *` arguments.

* :doc:`steps/step_03_hpy_final`: The completed port to HPy where all methods
  receive `HPy` handles and PyObject_HEAD has been removed.

Take a moment to read through :doc:`steps/step_00_c_api`. Then, once you're
ready, keep reading.


Step 01: Converting the module to a (legacy) HPy module
-------------------------------------------------------

First for the easy bit -- let's include `hpy.h`:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: #include <hpy.h>
    :end-at: #include <hpy.h>

We'd like to differentiate between references to `PyPointObject` that have
been ported to HPy and those that haven't, so let's rename it to `PointObject`
and alias `PyPointObject` to `PointObject`. We'll keep `PyPointObject` for
the instances that haven't been ported yet (the legacy ones) and use
`PointObject` where we have ported the references:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: typedef struct {
    :end-at: } PointObject;

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: typedef PointObject PyPointObject;
    :end-at: typedef PointObject PyPointObject;

For this step, all references will be to `PyPointObject` -- we'll only start
porting references in the next step.

Let's also call `HPyType_LEGACY_HELPERS` to define some helper functions
for use with the `PointObject` struct:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: HPyType_LEGACY_HELPERS(PointObject)
    :end-at: HPyType_LEGACY_HELPERS(PointObject)

Again, we won't use these helpers in this step -- we're just setting things
up for later.

Now for the big steps.

We need to replace `PyType_Spec` for the `Point` type with the equivalent
`HPyType_Spec`:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: // HPy type methods and slots (no methods or slots have been ported yet)
    :end-before: // Legacy module methods (the "dot" method is still a PyCFunction)

Initially the list of ported methods in `point_defines` is empty and all of
the methods are still in `Point_slots` which we have renamed to
`Point_legacy_slots` for clarity.

The `PointObject_IS_LEGACY` is defined by the `HPyType_LEGACY_HELPERS` macro
and will be set to true until we replace the legacy macro with the
`HPyType_HELPERS` one. Any type with `legacy_slots` or that still includes
`PyObject_HEAD` in its struct should have `legacy` set to true.

Similarly we must replace `PyModuleDef` with `HPyModuleDef`:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: // Legacy module methods (the "dot" method is still a PyCFunction)
    :end-before: HPy_MODINIT(step_01_hpy_legacy)

Like the type, the list of ported methods in `module_defines` is initially
empty and all the methods are still in `PointModuleMethods` which has
been renamed to `PointModuleLegacyMethods`.

Now all that is left is to replace the module initialization function with
one that uses `HPy_MODINIT`:

.. literalinclude:: steps/step_01_hpy_legacy.c
    :lineno-match:
    :start-at: HPy_MODINIT(step_01_hpy_legacy)

And we're done!

Note that the initialization function now takes an `HPyContext *` as an
argument and that this `ctx` is passed as the first argument to calls to
HPy API methods.

`PyModule_Create` is replaced with `HPyModule_Create` and `PyType_FromSpec`
is replaced by `HPyType_FromSpec`.

`HPy_SetAttr_s` is used to add the `Point` class to the module. HPy requires no
special `PyModule_AddObject` method.
