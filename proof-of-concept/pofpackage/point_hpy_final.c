#include <math.h>
#include <hpy.h>

// Porting to HPy, Step 2: Some legacy methods remain
//
// An example of porting a C extension that implements a Point type
// with a couple of simple methods (a norm and a dot product). It
// illustrates the steps needed to port types that contain additional
// C attributes (in this case, x and y).
//
// This file contains an example final step of the port in which allmethods
// have been converted to HPy methods and PyObject_HEAD has been removed.

typedef struct {
    // PyObject_HEAD is no longer required and has been removed. It's
    // like free extra RAM!
    int x;
    int y;
} HPyPointObject;

// Code that used to cast PyObject to PyPointObject relied on PyObject_HEAD
// as is no longer valid. The typedef below has been deleted to ensure that
// such code is detected by the compiler and can be ported.
// typedef HPyPointObject PyPointObject;

// The HPyPointObject_CAST macro will allow non-legacy methods to convert HPy
// handles to HPyPointObject structs. HPy_Cast is used because
// PyObject_HEAD is no longer present.
#define HPyPointObject_CAST(ctx, h) ((HPyPointObject*)HPy_Cast(ctx, h))
// TODO: Use HPyCast_DEFINE(HPyPointObject_Cast, HPyPointObject);

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

// this is an HPy function that uses Point
HPyDef_METH(...)
HPy dot(HPyContext ctx, HPy point1, HPy point2)
{
    HPyPointObject *p1 = HPyPointObject_CAST(ctx, point1);
    HPyPointObject *p2 = HPyPointObject_CAST(ctx, point2);
    double db;
    HPy result;
    dp = p1.x * p2.x + p1.y * p2.y;
    result = HPyFloat_FromDouble(dp);
    return result;
}

HPyType_Spec PointType_Spec = {
    .name = "Point",
    .legacy_headersize = offsetof(HPyPointObject, x),
    .legacy_slots = { ... }
}
