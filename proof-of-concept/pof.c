#include "hpy.h"


static PyObject* do_nothing(PyObject* self, PyObject* args)
{
    Py_RETURN_NONE;
}

static HPyMethodDef PofMethods[] = {
    {"do_nothing", do_nothing, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    "pof",                      /* m_name */
    "HPy Proof of Concept",     /* m_doc */
    -1,                         /* m_size */
    PofMethods                  /* m_methods */
};


HPY_MODINIT_FUNC(pof)(HPyContext ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (m == NULL)
        return NULL;
    return m;
}
