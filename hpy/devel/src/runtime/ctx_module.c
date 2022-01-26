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

/* This is a hack similar to what we do with types:
   We need some extra space to store our extra slots on
   the modules, the actual user data will be appended
   to this struct which we must align by using the union
   at the end.
*/
typedef struct {
    HPyFunc_traverseproc m_traverse_impl;
    HPyFunc_destroyfunc m_destroy_impl;
    union {
        unsigned char user_data_payload[1];
        max_align_t _align;
    };
} HPyMod_HEAD_t;

#define HPyMod_HEAD_SIZE (offsetof(HPyMod_HEAD_t, user_data_payload))

static int _decref_visitor(HPyField *pf, void *arg)
{
    PyObject *old_object = _hf2py(*pf);
    *pf = HPyField_NULL;
    Py_XDECREF(old_object);
    return 0;
}

static int hpymod_clear(PyObject *self)
{
    HPyMod_HEAD_t *extra = (HPyMod_HEAD_t*) PyModule_GetState(self);
    extra->m_traverse_impl(&extra->user_data_payload, _decref_visitor, NULL);
    return 0;
}

static void hpymod_free(void *self)
{
    HPyMod_HEAD_t *extra = (HPyMod_HEAD_t*) PyModule_GetState((PyObject*) self);
    if (extra->m_traverse_impl)
        extra->m_traverse_impl(&extra->user_data_payload, _decref_visitor, NULL);
    if (extra->m_destroy_impl)
        extra->m_destroy_impl(&extra->user_data_payload);
}

_HPy_HIDDEN void*
ctx_Module_GetState(HPyContext *ctx, HPy h_mod) {
    HPyMod_HEAD_t *internal = (HPyMod_HEAD_t *) PyModule_GetState(_h2py(h_mod));
    return internal ? &internal->user_data_payload : NULL;
}

_HPy_HIDDEN HPy
ctx_Module_Create(HPyContext *ctx, HPyModuleDef *hpydef)
{
    // create a new PyModuleDef

    // we can't free this memory because it is stitched into moduleobject. We
    // just make it immortal for now, eventually we should think whether or
    // not to free it if/when we unload the module
    PyModuleDef *def = (PyModuleDef*)PyMem_Malloc(sizeof(PyModuleDef));
    if (def == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    memcpy(def, &empty_moduledef, sizeof(PyModuleDef));
    def->m_name = hpydef->name;
    def->m_doc = hpydef->doc;
    def->m_methods = create_method_defs(hpydef->defines, hpydef->legacy_methods);

    HPyMod_HEAD_t extra = { .m_traverse_impl = NULL, .m_destroy_impl = NULL };
    bool needs_state = hpydef->size != -1;
    if (hpydef->defines != NULL) {
        for (int i = 0; hpydef->defines[i] != NULL; i++) {
            HPyDef *src = hpydef->defines[i];
            if (src->kind != HPyDef_Kind_Slot)
                continue;
            if (src->slot.slot == HPy_m_destroy) {
                extra.m_destroy_impl = (HPyFunc_destroyfunc) src->slot.impl;
                // No trampoline, this is HPy specific slot not called by
                // CPython runtime, but by us in hpymod_free
                continue;
            }
            if (src->slot.slot == HPy_m_traverse) {
                def->m_traverse = (traverseproc) src->slot.cpy_trampoline;
                extra.m_traverse_impl = (HPyFunc_traverseproc) src->slot.impl;
                def->m_clear = &hpymod_clear;
            } else {
                const size_t buffer_size = 256;
                char buffer[buffer_size];
                snprintf(buffer, buffer_size, "Unsupported slot number %d for module '%s'", src->slot.slot, hpydef->name);
                HPy_FatalError(ctx, buffer);
            }
        }
    }

    if (needs_state) {
        // The assumption is such that most of the time if there is a module
        // state, then there will be some HPyFields to traverse, so we always
        // allocate our custom state. This way HPyModule_GetState can assume
        // that state is always HPyMod_Extra_t.
        def->m_size = HPyMod_HEAD_SIZE + hpydef->size;
        def->m_free = &hpymod_free;
    }

    if (def->m_methods == NULL) {
        PyMem_Free(def);
        return HPy_NULL;
    }
    PyObject *result = PyModule_Create(def);
    if (needs_state) {
        HPyMod_HEAD_t *state = (HPyMod_HEAD_t*) PyModule_GetState(result);
        state->m_traverse_impl = extra.m_traverse_impl;
        state->m_destroy_impl = extra.m_destroy_impl;
    }
    return _py2h(result);
}

_HPy_HIDDEN int call_mod_traverseproc_from_trampoline(HPyFunc_traverseproc tp_traverse,
                                                      PyObject *self,
                                                      cpy_visitproc cpy_visit,
                                                      void *cpy_arg)
{
    return call_struct_traverseproc_from_trampoline(tp_traverse,
                                                    &((HPyMod_HEAD_t*) PyModule_GetState(self))->user_data_payload,
                                                    cpy_visit,
                                                    cpy_arg);
}