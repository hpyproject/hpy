#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

_HPy_HIDDEN HPy
ctx_Capsule_New(HPyContext *ctx, void *pointer, const char *name)
{
    return _py2h(PyCapsule_New(pointer, name, NULL));
}

_HPy_HIDDEN void *
ctx_Capsule_Get(HPyContext *ctx, HPy capsule, HPyCapsule_key key, const char *name)
{
    switch (key)
    {
    case HPyCapsule_key_Pointer:
        return PyCapsule_GetPointer(_h2py(capsule), name);
    case HPyCapsule_key_Name:
        return (void *) PyCapsule_GetName(_h2py(capsule));
    case HPyCapsule_key_Context:
        return PyCapsule_GetContext(_h2py(capsule));
    }
    /* unreachable */
    assert(0);
    return NULL;
}

_HPy_HIDDEN int
ctx_Capsule_Set(HPyContext *ctx, HPy capsule, HPyCapsule_key key, void *value)
{
    switch (key)
    {
    case HPyCapsule_key_Pointer:
        return PyCapsule_SetPointer(_h2py(capsule), value);
    case HPyCapsule_key_Name:
        return PyCapsule_SetName(_h2py(capsule), (const char *) value);
    case HPyCapsule_key_Context:
        return PyCapsule_SetContext(_h2py(capsule), value);
    }
    /* unreachable */
    assert(0);
    return -1;
}
