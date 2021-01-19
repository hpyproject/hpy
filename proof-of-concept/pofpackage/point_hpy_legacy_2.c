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
    double x;
    double y;
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

// this is a method for creating a Point
// XXX: Port this
int Point_init(PyObject *self, PyObject *args, PyObject *kw)
{
    static char *kwlist[] = {"x", "y", NULL};
    PyPointObject *p = (PyPointObject *)self;
    p->x = 0.0;
    p->y = 0.0;
    if (!PyArg_ParseTupleAndKeywords(args, kw, "|dd", kwlist,
                                     &p->x, &p->y))
        return -1;
    return 0;
}

// an HPy method of Point
HPyDef_METH(Point_norm, "noargs", Point_norm_impl, HPyFunc_NOARGS, .doc="Distance from origin.")
HPy Point_norm_impl(HPyContext ctx, HPy self)
{
    HPyPointObject *p = HPyPointObject_CAST(ctx, self);
    double norm;
    HPy result;
    norm = sqrt(p->x * p->x + p->y * p->y);
    result = HPyFloat_FromDouble(ctx, norm);
    return result;
}

// this is an LEGACY function which casts a PyObject* into a PyPointObject*
PyObject* dot(PyObject *self, PyObject *args)
{
    PyObject *point1, *point2;
    if (!PyArg_ParseTuple(args, "OO",  &point1, &point2))
        return NULL;

    PyPointObject *p1 = (PyPointObject *)point1;
    PyPointObject *p2 = (PyPointObject *)point2;

    double dp;
    PyObject *result;
    dp = p1->x * p2->x + p1->y * p2->y;
    result = PyFloat_FromDouble(dp);
    return result;
}

// Method, type and module definitions. In this porting step .norm()
// is ported to HPy, but dot(...) remains a legacy methods.
// Point.__init__ and Point.__doc__ are ported from legacy slots to
// HPy type defines.

// Legacy methods (there are no legacy methods left now)
static PyMethodDef PointMethods[] = {
    {NULL, NULL, 0, NULL}
};

// Legacy slots (all slots are still legacy slots)
static PyType_Slot Point_legacy_slots[] = {
    {Py_tp_doc, "Point (Step 1; All legacy methods)"},
    {Py_tp_init, Point_init},
    {Py_tp_methods, PointMethods},
    {0, 0}
};

// HPy type methods and slots (no methods or slots have been ported yet)
static HPyDef *point_defines[] = {
    &Point_norm,
    NULL
};

static HPyType_Spec Point_Type_spec = {
    .name = "point_capi.Point",
    .basicsize = sizeof(PyPointObject),
    .itemsize = 0,
    .flags = HPy_TPFLAGS_DEFAULT,
    .legacy_slots = Point_legacy_slots,
    .defines = point_defines
};

// Legacy module methods (the "dot" method is still a PyCFunction)
static PyMethodDef PointModuleLegacyMethods[] = {
    {"dot", (PyCFunction)dot, METH_VARARGS, "Dot product."},
    {NULL, NULL, 0, NULL}
};

// HPy module methods (no methods have been ported yet)
static HPyDef *module_defines[] = {
    NULL
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "point_hpy_legacy_2",
    .m_doc = "Point module (Step 2; Some legacy methods remain)",
    .m_size = -1,
    .legacy_methods = PointModuleLegacyMethods,
    .defines = module_defines,
};

HPy_MODINIT(point_hpy_legacy_2)
static HPy init_point_hpy_legacy_2_impl(HPyContext ctx)
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
