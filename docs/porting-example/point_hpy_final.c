#include <math.h>
#include <hpy.h>

// Porting to HPy, Step 3: All methods ported
//
// An example of porting a C extension that implements a Point type
// with a couple of simple methods (a norm and a dot product). It
// illustrates the steps needed to port types that contain additional
// C attributes (in this case, x and y).
//
// This file contains an example final step of the port in which all methods
// have been converted to HPy methods and PyObject_HEAD has been removed.

typedef struct {
    // PyObject_HEAD is no longer available in PointObject. In CPython,
    // of course, it still exists but is inaccessible from HPy_AsStruct. In
    // other Python implementations (e.g. PyPy) it might no longer exist at
    // all.
    double x;
    double  y;
} PointObject;

// Code using PyPointObject relied on PyObject_HEAD and is no longer valid
// (PyObject_HEAD has been removed from the PointObject struct above). The
// typedef below has been deleted to ensure that such code is now generates
// an error during compilation.
// typedef PointObject PyPointObject;

// The type helper macro defines an PointObject_AsStruct function allows
// converting HPy handles to PointObject structs. We no longer need to use
// the legacy type helper macro because PyObject_HEAD has been removed from
// PointObject.
HPyType_HELPERS(PointObject)

// this is a method for creating a Point
HPyDef_SLOT(Point_init, Point_init_impl, HPy_tp_init)
int Point_init_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs, HPy kw)
{
    static const char *kwlist[] = {"x", "y", NULL};
    PointObject *p = PointObject_AsStruct(ctx, self);
    p->x = 0.0;
    p->y = 0.0;
    if (!HPyArg_ParseKeywords(ctx, NULL, args, nargs, kw, "|dd", kwlist,
                              &p->x, &p->y))
        return -1;
    return 0;
}

// an HPy method of Point
HPyDef_METH(Point_norm, "norm", Point_norm_impl, HPyFunc_NOARGS, .doc="Distance from origin.")
HPy Point_norm_impl(HPyContext ctx, HPy self)
{
    PointObject *p = PointObject_AsStruct(ctx, self);
    double norm;
    HPy result;
    norm = sqrt(p->x * p->x + p->y * p->y);
    result = HPyFloat_FromDouble(ctx, norm);
    return result;
}

// this is an HPy function that uses Point
HPyDef_METH(dot, "dot", dot_impl, HPyFunc_VARARGS, .doc="Dot product.")
HPy dot_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    HPy point1, point2;
    if (!HPyArg_Parse(ctx, NULL, args, nargs, "OO",  &point1, &point2))
        return HPy_NULL;
    PointObject *p1 = PointObject_AsStruct(ctx, point1);
    PointObject *p2 = PointObject_AsStruct(ctx, point2);
    double dp;
    HPy result;
    dp = p1->x * p2->x + p1->y * p2->y;
    result = HPyFloat_FromDouble(ctx, dp);
    return result;
}

// Method, type and module definitions. In this porting step all
// methods and slots have been ported to HPy and all legacy support
// has been removed.

// Support for legacy methods and slots has been removed. It used to be:
///
// static PyMethodDef PointMethods[] = { ... }
// static PyType_Slot Point_legacy_slots[] = { ... }
// static PyMethodDef PointModuleLegacyMethods[] = { ... }
//
// and .legacy_slots and .legacy_defines have been removed from HPyType_Spec
// HPyModuleDef respectively.

// HPy type methods and slots (no methods or slots have been ported yet)
static HPyDef *point_defines[] = {
    &Point_init,
    &Point_norm,
    NULL
};

static HPyType_Spec Point_Type_spec = {
    .name = "point_hpy_final.Point",
    .basicsize = sizeof(PointObject),
    .itemsize = 0,
    .flags = HPy_TPFLAGS_DEFAULT,
    .defines = point_defines
};

// HPy module methods (no methods have been ported yet)
static HPyDef *module_defines[] = {
    &dot,
    NULL
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "point_hpy_legacy_2",
    .m_doc = "Point module (Step 2; Porting some methods)",
    .m_size = -1,
    .defines = module_defines,
};

HPy_MODINIT(point_hpy_final)
static HPy init_point_hpy_final_impl(HPyContext ctx)
{
    HPy m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;

    HPy point_type = HPyType_FromSpec(ctx, &Point_Type_spec, NULL);
    if (HPy_IsNull(point_type))
      return HPy_NULL;
    HPy_SetAttr_s(ctx, m, "Point", point_type);

    return m;
}
