#include <math.h>
#include <Python.h>
#include <hpy.h>

// Porting to HPy, Step 2: Some legacy methods remain
//
// An example of porting a C extension that implements a Point type
// with a couple of simple methods (a norm and a dot product). It
// illustrates the steps needed to port types that contain additional
// C attributes (in this case, x and y).
//
// This file contains an example second step of the port in which some methods
// have been converted to HPy methods that receive handles as arguments, but
// other methods are still legacy methods that receive PyObject arguments.

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
// handles to HPyPointObject structs. HPy_CastLegacy is used because
// PyObject_HEAD is still present.
#define HPyPointObject_CAST(ctx, h) ((HPyPointObject*)HPy_CastLegacy(ctx, h))
// TODO: Use HPyCast_DEFINE_LEGACY(HPyPointObject_Cast, HPyPointObject);

// an HPy method of Point
HPyDef_METH(...)
HPy Point_norm(HPyContext ctx, HPy self)
{
    HPyPointObject *p = HPyPointObject_CAST(ctx, self);
    double norm;
    HPy result;
    norm = sqrt(p.x * p.x + p.y * p.y);
    result = HPyFloat_FromDouble(ctx, norm);
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
