#include <Python.h>
#include "hpy.h"
#include "hpy/runtime/ctx_type.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

static PyModuleDef empty_moduledef = {
    PyModuleDef_HEAD_INIT
};

_HPy_HIDDEN PyModuleDef *create_module_def(HPyModuleDef *hpydef, void *create_func, void *exec_func)
{
    // we can't free this memory because it is stitched into moduleobject. We
    // just make it immortal for now, eventually we should think whether or
    // not to free it if/when we unload the module
    PyModuleDef *def = (PyModuleDef*)PyMem_Malloc(sizeof(PyModuleDef));
    if (def == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    memcpy(def, &empty_moduledef, sizeof(PyModuleDef));
    def->m_name = hpydef->name;
    def->m_doc = hpydef->doc;
    def->m_size = hpydef->size;

    size_t slots_count = 0;
    if (create_func != NULL)
        slots_count++;
    if (exec_func != NULL)
        slots_count++;
    if (slots_count > 0) {
        PyModuleDef_Slot *slots = PyMem_Malloc(sizeof(PyModuleDef_Slot) * (slots_count + 1));
        slots[slots_count] = (PyModuleDef_Slot) {0, NULL};
        size_t slot_index = 0;
        if (create_func != NULL)
            slots[slot_index++] = (PyModuleDef_Slot){Py_mod_create, create_func};
        if (exec_func != NULL)
            slots[slot_index++] = (PyModuleDef_Slot){Py_mod_exec, exec_func};
        def->m_slots = slots;
    } else {
        def->m_slots = NULL;
    }
    def->m_methods = create_method_defs(hpydef->defines, hpydef->legacy_methods);
    if (def->m_methods == NULL) {
        PyMem_Free(def);
        return NULL;
    }
    return def;
}

_HPy_HIDDEN HPy
ctx_Module_Create(HPyContext *ctx, HPyModuleDef *hpydef)
{
    // create a new PyModuleDef
    PyModuleDef *def = create_module_def(hpydef, NULL, NULL);
    PyObject *result = PyModule_Create(def);
    return _py2h(result);
}
