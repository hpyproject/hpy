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
