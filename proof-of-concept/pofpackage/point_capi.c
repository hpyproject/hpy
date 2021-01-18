#include <math.h>
#include <Python.h>

// Porting to HPy, Step 0: Original Python C API version
//
// An example of porting a C extension that implements a Point type
// with a couple of simple methods (a norm and a dot product). It
// illustrates the steps needed to port types that contain additional
// C attributes (in this case, x and y).
//
// This file contains the original C API version that needs to be ported.
//
// HPy supports porting C extensions piece by piece.
//
// point_hpy_legacy_1.c illustrates a possible first step where all
// methods still receive PyObject arguments and may still cast them to
// PyPointObject if they are instances of Point.
//
// point_hpy_legacy_2.c shows how to transition some methods to HPy methods
// that receive HPy handles as arguments while still supporting legacy
// methods that receive PyObject arguments.
//
// point_hpy_final.c shows the completed port to HPy where all methods receive
// HPy handles and PyObject_HEAD has been removed.

typedef struct {
    PyObject_HEAD
    double x;
    double y;
} PyPointObject;

// this is a method of Point
PyObject* Point_norm(PyObject *self)
{
    PyPointObject *p = (PyPointObject *)self;
    double norm;
    PyObject result;
    norm = sqrt(p.x * p.x + p.y * p.y);
    result = PyFloat_FromDouble(norm);
    return result;
}

// this is an unrelated function which happens to cast a PyObject* into a
// PyPointObject*
PyObject dot(PyObject *point1, PyObject *point2)
{
    PyPointObject *p1 = (PyPointObject *)point1;
    PyPointObject *p2 = (PyPointObject *)point2;
    double db;
    PyObject result;
    dp = p1.x * p2.x + p1.y * p2.y;
    result = PyFloat_FromDouble(dp);
    return result;
}

PyType_Spec PointType_Spec = {
    .name = "Point",
}
