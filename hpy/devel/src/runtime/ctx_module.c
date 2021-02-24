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


static int create_module_typespecs(HPyContext ctx, HPy mod, HPyDef *hpydefs[])
{
    HPy h_type;
    if (hpydefs != NULL) {
        for(int i=0; hpydefs[i] != NULL; i++) {
            HPyDef *src = hpydefs[i];
            if (src->kind != HPyDef_Kind_TypeSpecDef)
                continue;
            h_type = HPyType_FromSpec(ctx, &src->type.typespec, NULL);
            if (HPy_IsNull(h_type)) {
                // XXX: set error
                return -1;
            }
            if (HPy_SetAttr_s(ctx, mod, src->type.name, h_type) != 0) {
                HPy_Close(ctx, h_type);
                // XXX: set error
                return -1;
            }
            HPy_Close(ctx, h_type);
        }
    }
    return 0;
}


_HPy_HIDDEN HPy
ctx_Module_Create(HPyContext ctx, HPyModuleDef *hpydef)
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
    PyObject *result = PyModule_Create(def);
    HPy mod = _py2h(result);
    if (!HPy_IsNull(mod)) {
         if (create_module_typespecs(ctx, mod, hpydef->defines) < 0) {
             HPy_Close(ctx, mod); // this will also Py_DECREF result
             PyMem_Free(def);
             return HPy_NULL;
         }
    }
    return mod;
}
