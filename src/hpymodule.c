#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject* set_debug(PyObject* self, PyObject* args)
{
    // TODO
    Py_RETURN_NONE;
}


static PyMethodDef HPyMethods[] = {
    {"set_debug", (PyCFunction)set_debug, METH_O, "TODO"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef hpydef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "hpy",
    .m_doc = "HPy",
    .m_size = 0,
    .m_methods = HPyMethods,
};


PyMODINIT_FUNC
PyInit_hpy(void)
{
    return PyModuleDef_Init(&hpydef);
}
