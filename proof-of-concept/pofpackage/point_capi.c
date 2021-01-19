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

// this is a method for creating a Point
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

// this is a method of Point
PyObject* Point_norm(PyObject *self)
{
    PyPointObject *p = (PyPointObject *)self;
    double norm;
    PyObject *result;
    norm = sqrt(p->x * p->x + p->y * p->y);
    result = PyFloat_FromDouble(norm);
    return result;
}

// this is an unrelated function which happens to cast a PyObject* into a
// PyPointObject*
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


// Boiler plate for defining the type an

static PyMethodDef PointMethods[] = {
    {"norm", (PyCFunction)Point_norm, METH_NOARGS, "Distance from origin."},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot Point_slots[] = {
    {Py_tp_doc, "Point (Step 0; C API implementation)"},
    {Py_tp_init, Point_init},
    {Py_tp_methods, PointMethods},
    {0, 0}
};

static PyType_Spec Point_Type_spec = {
    .name = "point_capi.Point",
    .basicsize = sizeof(PyPointObject),
    .itemsize = 0,
    .flags = Py_TPFLAGS_DEFAULT,
    .slots = Point_slots
};

static PyMethodDef PointModuleMethods[] = {
    {"dot", (PyCFunction)dot, METH_VARARGS, "Dot product."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "point_capi",
    "Point module (Step 0; C API implementation)",
    -1,
    PointModuleMethods,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC
PyInit_point_capi(void)
{
    PyObject* m;
    m = PyModule_Create(&moduledef);
    if (m == NULL)
        return NULL;

    PyObject *point_type = PyType_FromSpec(&Point_Type_spec);
    if (point_type == NULL)
        return NULL;
    PyModule_AddObject(m, "Point", point_type);

    return m;
}
