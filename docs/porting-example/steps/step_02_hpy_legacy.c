#include <math.h>
#include <Python.h>
#include <hpy.h>

// Porting to HPy, Step 2: Porting some methods
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
    // PointObject and should be removed once the port to HPy is completed.
    PyObject_HEAD
    double x;
    double y;
    // HPy handles are shortlived to support all GC strategies
    // For that reason, PyObject* in C structs are replaced by HPyField
    HPyField obj;
} PointObject;

// This defines PyPointObject as an alias of PointObject so that existing
// code that still uses PyPointObject and expects PyObject_HEAD continues to
// compile and run. Once PyObject_HEAD has been removed, this alias should be
// removed so that code that still expects PyObject_HEAD will fail to compile.
typedef PointObject PyPointObject;

// The legacy type helper macro defines an PointObject_AsStruct function allows
// non-legacy methods to convert HPy handles to PointObject structs. The legacy
// type helper macro is used because PyObject_HEAD is still present in
// PointObject. Once PyObject_HEAD has been removed (see point_hpy_final.c) we
// will use HPy_TYPE_HELPERS instead.
HPyType_LEGACY_HELPERS(PointObject)

HPyDef_SLOT(Point_traverse, HPy_tp_traverse)
int Point_traverse_impl(void *self, HPyFunc_visitproc visit, void *arg)
{
    HPy_VISIT(&((PointObject*)self)->obj);
    return 0;
}

// this is a method for creating a Point
HPyDef_SLOT(Point_init, HPy_tp_init)
int Point_init_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs, HPy kw)
{
    static const char *kwlist[] = {"x", "y", "obj", NULL};
    PointObject *p = PointObject_AsStruct(ctx, self);
    p->x = 0.0;
    p->y = 0.0;
    HPy obj = HPy_NULL;
    HPyTracker ht;
    if (!HPyArg_ParseKeywords(ctx, &ht, args, nargs, kw, "|ddO", kwlist,
                              &p->x, &p->y, &obj))
        return -1;
    if (HPy_IsNull(obj))
        obj = ctx->h_None;
    // INCREF not needed because HPyArg_ParseKeywords does not steal a reference
    HPyField_Store(ctx, self, &p->obj, obj);
    HPyTracker_Close(ctx, ht);
    return 0;
}

// this is the getter for the associated object
HPyDef_GET(Point_obj, "obj", .doc="Associated object.")
HPy Point_obj_get(HPyContext *ctx, HPy self, void* closure)
{
    PointObject *p = PointObject_AsStruct(ctx, self);
    return HPyField_Load(ctx, self, p->obj);
}

// an HPy method of Point
HPyDef_METH(Point_norm, "norm", HPyFunc_NOARGS, .doc="Distance from origin.")
HPy Point_norm_impl(HPyContext *ctx, HPy self)
{
    PointObject *p = PointObject_AsStruct(ctx, self);
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
    {Py_tp_doc, "Point (Step 2; Porting some methods)"},
    {Py_tp_methods, PointMethods},
    {0, 0}
};

// HPy type methods and slots
static HPyDef *point_defines[] = {
    &Point_init,
    &Point_norm,
    &Point_obj,
    &Point_traverse,
    NULL
};

static HPyType_Spec Point_Type_spec = {
    .name = "point_hpy_legacy_2.Point",
    .basicsize = sizeof(PointObject),
    .itemsize = 0,
    .flags = HPy_TPFLAGS_DEFAULT,
    .builtin_shape = SHAPE(PointObject),
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
    .name = "step_02_hpy_legacy",
    .doc = "Point module (Step 2; Porting some methods)",
    .size = -1,
    .legacy_methods = PointModuleLegacyMethods,
    .defines = module_defines,
};

HPy_MODINIT(step_02_hpy_legacy)
static HPy init_step_02_hpy_legacy_impl(HPyContext *ctx)
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
