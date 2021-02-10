#ifndef HPY_UNIVERSAL_HPYTYPE_H
#define HPY_UNIVERSAL_HPYTYPE_H

#include <stdbool.h>

typedef struct {
    const char* name;
    int basicsize;
    int itemsize;
    unsigned long flags;
    int legacy;
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
#define HPy_TPFLAGS_DEFAULT (_Py_TPFLAGS_HEAPTYPE | _Py_TPFLAGS_HAVE_VERSION_TAG)

/* HPy_TPFLAGS_LEGACY is set automatically on legacy types.

   A custom type is a legacy type if its struct begins with PyObject_HEAD and
   it must set .legacy = 1 in its HPyType_Spec.

   A type with .legacy_slots not NULL is required to have .legacy = 1 and to
   include PyObject_HEAD at the start of its struct. It would be easy to
   relax this requirement on CPython (where the PyObject_HEAD fields are
   always present) but a large burden on other implementations (e.g. PyPy,
   GraalPython) where a struct starting with PyObject_HEAD might not exist.

   Types that do not define a struct of their own, should set the value of
   .legacy to the same value as the type they inheret from. If they inherit
   from a built-in type, they may .legacy to either 0 or 1, depending on
   whether they still use .legacy_slots or not.

   Note on the choice of bit 8: Bit 8 looks likely to be the last free TPFLAG
   bit that C Python will allocate. Bits 0 to 8 were dropped in Python 3.0 and
   are being re-allocated slowly from 0 towards 8. As of 3.10, only bit 0 has
   been re-allocated.
*/
#define HPy_TPFLAGS_LEGACY (1UL << 8)

/* Set if the type allows subclassing */
#define HPy_TPFLAGS_BASETYPE (1UL << 10)

#endif /* HPY_UNIVERSAL_HPYTYPE_H */
