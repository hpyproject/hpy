#ifndef HPY_UNIVERSAL_HPYTYPE_H
#define HPY_UNIVERSAL_HPYTYPE_H

#include <stdbool.h>

/* To be able to be compatible with CPython (in both CPython and Universal ABI
   modes) we need to reserve some space at the beginning of user-defined
   structs which will contain ob_refcnt and ob_type. However, HPy users should
   not be able to access them directly.

   Alternate implementations are free to use this extra space as they
   want. Moreover, if they do NOT need this extra space, they can avoid
   wasting memory by allocating sizeof(_HPyObject_head_s) bytes less inside
   their implementation of HPy_New.
*/


struct _HPyObject_head_s {
    HPy_ssize_t _reserved0;
    void *_reserved1;
};
#define HPyObject_HEAD struct _HPyObject_head_s _ob_head;


typedef struct {
    const char* name;
    int basicsize;
    int itemsize;
    unsigned int flags;
    int legacy_headersize;
    void *legacy_slots; // PyType_Slot *
    HPyDef **defines;   /* points to an array of 'HPyDef *' */
} HPyType_Spec;

typedef enum {
    HPyType_SpecParam_Base = 1,
    HPyType_SpecParam_BasesTuple = 2,
    //HPyType_SpecParam_Metaclass = 3,
    //HPyType_SpecParam_Module = 4,
} HPyType_SpecParam_Kind;

typedef struct {
    HPyType_SpecParam_Kind kind;
    HPy object;
} HPyType_SpecParam;

/* All types are dynamically allocated */
#define _Py_TPFLAGS_HEAPTYPE (1UL << 9)
#define _Py_TPFLAGS_HAVE_VERSION_TAG (1UL << 18)


/* HPy_TPFLAGS_LEGACY is set automatically on legacy types.

   A type is a legacy type if legacy_headersize is greater than zero. This
   is typically true if HPyObject_HEAD is included in the type struct
   and the type sets legacy_* slots or methods.

   Note on the choice of bit 8: Bit 8 looks likely to be the last free TPFLAG
   bit that C Python will allocate. Bits 0 to 8 were dropped in Python 3.0 and
   are being re-allocated slowly from 0 towards 8. As of 3.10, only bit 0 has
   been re-allocated.
*/
#define HPy_TPFLAGS_LEGACY (1UL << 8)

/* Set if the type allows subclassing */
#define HPy_TPFLAGS_BASETYPE (1UL << 10)

#define HPy_TPFLAGS_DEFAULT (_Py_TPFLAGS_HEAPTYPE | _Py_TPFLAGS_HAVE_VERSION_TAG)

#define HPy_Cast(ctx, h) (_HPy_Cast(ctx, h) + sizeof(struct {HPyObject_HEAD}))
#define HPy_CastLegacy(ctx, h) (_HPy_Cast(ctx, h))

#endif /* HPY_UNIVERSAL_HPYTYPE_H */
