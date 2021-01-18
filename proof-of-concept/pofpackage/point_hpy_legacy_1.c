#include <math.h>
#include <Python.h>
#include <hpy.h>

// Porting to HPy, Step 1: All legacy methods
//
// An example of porting a C extension that implements a Point type
// with a couple of simple methods (a norm and a dot product). It
// illustrates the steps needed to port types that contain additional
// C attributes (in this case, x and y).
//
// This file contains an example first step of the port in which all methods
// still receive PyObject arguments and may still cast them to
// PyPointObject if they are instances of Point.

typedef struct {
    // PyObject_HEAD is required while legacy methods still access
    // HPyPointObject and should be removed once the port to HPy is completed.
    PyObject_HEAD
    int x;
    int y;
} HPyPointObject;

// This defines PyPointObject as an alias of HPyPointObject so that existing
// code that still uses PyPointObject and expects PyObject_HEAD continues to
// compile and run. Once PyObject_HEAD has been removed, this alias should be
// removed so that code that still expects PyObject_HEAD will fail to compile.
typedef HPyPointObject PyPointObject;

// The HPyPointObject_CAST macro will allow non-legacy methods to convert HPy
// handles to HPyPointObject structs. It is not used in this file, but
// is provided so that methods can start to be ported (see
// point_hpy_legacy_2.c). HPy_CastLegacy is used because PyObject_HEAD is still
// present.
#define HPyPointObject_CAST(ctx, h) ((HPyPointObject*)HPy_CastLegacy(ctx, h))
// TODO: Use HPyCast_DEFINE_LEGACY(HPyPointObject_Cast, HPyPointObject);

// this is a LEGACY method of Point
PyObject* Point_norm(PyObject *self)
{
    PyPointObject *p = (PyPointObject *)self; // still works
    double norm;
    PyObject result;
    norm = sqrt(p.x * p.x + p.y * p.y);
    result = PyFloat_FromDouble(norm);
    return result;
}

// this is an LEGACY function which casts a PyObject* into a PyPointObject*
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

HPyType_Spec PointType_Spec = {
    .name = "Point",
    .legacy_headersize = offsetof(HPyPointObject, x),
    .legacy_slots = { ... }
}
