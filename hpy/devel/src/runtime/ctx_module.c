#include <Python.h>
#include "hpy.h"
#include "common/runtime/ctx_type.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

static PyModuleDef empty_moduledef = {
    PyModuleDef_HEAD_INIT
};

static HPy_ssize_t
HPyDef_count(HPyDef *defs[], HPyDef_Kind kind)
{
    HPy_ssize_t res = 0;
    if (defs == NULL)
        return res;
    for(int i=0; defs[i] != NULL; i++)
        if (defs[i]->kind == kind)
            res++;
    return res;
}

static int
hpy_module_slot_to_cpy_slot(HPyModule_Slot src)
{
    return src;        /* same numeric value by default */
}

static PyModuleDef_Slot *
create_moduleslot_defs(HPyDef *hpydefs[])
{
    HPy_ssize_t hpymoduleslot_count = HPyDef_count(hpydefs,
                                                   HPyDef_Kind_ModuleSlot);

    PyModuleDef_Slot *result = PyMem_Calloc(hpymoduleslot_count + 1,
                                            sizeof(PyModuleDef_Slot));
    if (result == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    int dst_idx = 0;
    if (hpydefs != NULL) {
        for (int i = 0; hpydefs[i] != NULL; i++) {
            HPyDef *src = hpydefs[i];
            if (src->kind != HPyDef_Kind_ModuleSlot)
                continue;
            PyModuleDef_Slot *dst = &result[dst_idx++];
            dst->slot = hpy_module_slot_to_cpy_slot(src->module_slot.slot);
            dst->value = src->module_slot.cpy_trampoline;
        }
    }

    // add the NULL sentinel at the end
    result[dst_idx++] = (PyModuleDef_Slot){0, NULL};

    return result;
}

_HPy_HIDDEN HPy
ctx_Module_Create(HPyContext *ctx, HPyModuleDef *hpydef)
{
    // create a new PyModuleDef

    // we can't free this memory because it is stitched into moduleobject. We
    // just make it immortal for now, eventually we should think whether or
    // not to free it if/when we unload the module
    PyModuleDef *def = PyMem_Malloc(sizeof(PyModuleDef));
    if (def == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    memcpy(def, &empty_moduledef, sizeof(PyModuleDef));
    def->m_name = hpydef->m_name;
    def->m_doc = hpydef->m_doc;
    def->m_size = hpydef->m_size;
    def->m_methods = create_method_defs(hpydef->defines, hpydef->legacy_methods);
    if (def->m_methods == NULL) {
        PyMem_Free(def);
        return HPy_NULL;
    }
    def->m_slots = create_moduleslot_defs(hpydef->defines);

    // XXX: Add a check that the isn't anything in hpydef->defines other than
    //      method defs and module slots. E.g.
    // if (dst_idx != total_slot_count + 1)
    //      Py_FatalError("bogus slot count in create_slot_defs");

    PyObject *result = PyModule_Create(def);
    return _py2h(result);
}
