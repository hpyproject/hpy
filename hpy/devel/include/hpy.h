#ifndef HPy_H
#define HPy_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

/* ~~~~~~~~~~~~~~~~ useful macros ~~~~~~~~~~~~~~~~ */

#ifdef __GNUC__
#define _HPy_HIDDEN  __attribute__((visibility("hidden")))
#else
#define _HPy_HIDDEN
#endif /* __GNUC__ */

#if defined(__clang__) || \
    (defined(__GNUC__) && \
     ((__GNUC__ >= 3) || \
      (__GNUC__ == 2) && (__GNUC_MINOR__ >= 5)))
#  define _HPy_NO_RETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#  define _HPy_NO_RETURN __declspec(noreturn)
#else
#  define _HPy_NO_RETURN
#endif

#define HPyAPI_RUNTIME_FUNC(restype) _HPy_HIDDEN restype

/* ~~~~~~~~~~~~~~~~ Definition of the type HPy ~~~~~~~~~~~~~~~~ */

/* HPy handles are fully opaque: depending on the implementation, the _i can
   be either an integer or a pointer. A few examples:

   * in CPython ABI mode, ._i is a PyObject*

   * in Universal ABI mode, the meaning of ._i depends on the implementation:

       - CPython (i.e., the code in hpy/universal/src/): ._i is the bitwise
         invert of a PyObject*

       - PyPy: ._i is an index into a list

       - GraalPython: ???

       - Debug mode: _i is a pointer to a DebugHandle, which contains a
         another HPy among other stuff
 */
typedef struct _HPy_s { intptr_t _i; } HPy;
typedef struct { intptr_t _lst; } HPyListBuilder;
typedef struct { intptr_t _tup; } HPyTupleBuilder;
typedef struct { intptr_t _i; } HPyTracker;


/* A null handle is officially defined as a handle whose _i is 0. This is true
   in all ABI modes. */
#define HPy_NULL ((HPy){0})
#define HPy_IsNull(h) ((h)._i == 0)

/* Convenience functions to cast between HPy and void*.  We need to decide
   whether these are part of the official API or not, and maybe introduce a
   better naming convetion. For now, they are needed for ujson. */
static inline HPy HPy_FromVoidP(void *p) { return (HPy){(intptr_t)p}; }
static inline void* HPy_AsVoidP(HPy h) { return (void*)h._i; }


/* ~~~~~~~~~~~~~~~~ Definition of other types ~~~~~~~~~~~~~~~~ */

typedef struct _HPyContext_s HPyContext;

#ifdef HPY_UNIVERSAL_ABI
    typedef intptr_t HPy_ssize_t;
    typedef intptr_t HPy_hash_t;
#else
/*  It would be nice if we could include hpy.h WITHOUT bringing in all the
    stuff from Python.h, to make sure that people don't use the CPython API by
    mistake. How to achieve it, though? */
#   define PY_SSIZE_T_CLEAN
#   include <Python.h>
    typedef Py_ssize_t HPy_ssize_t;
    typedef Py_hash_t HPy_hash_t;
#endif


/* ~~~~~~~~~~~~~~~~ Additional #includes ~~~~~~~~~~~~~~~~ */

/* compatibility CPython types */
#include "hpy/cpy_types.h"
#include "hpy/macros.h"
#include "hpy/hpyfunc.h"
#include "hpy/hpydef.h"
#include "hpy/hpytype.h"
#include "hpy/hpymodule.h"
#include "hpy/runtime/argparse.h"
#include "hpy/runtime/helpers.h"

#ifdef HPY_UNIVERSAL_ABI
#   include "hpy/universal/autogen_ctx.h"
#   include "hpy/universal/autogen_trampolines.h"
#   include "hpy/universal/misc_trampolines.h"
#else
//  CPython-ABI
#   include "hpy/runtime/ctx_call.h"
#   include "hpy/runtime/ctx_module.h"
#   include "hpy/runtime/ctx_object.h"
#   include "hpy/runtime/ctx_type.h"
#   include "hpy/runtime/ctx_listbuilder.h"
#   include "hpy/runtime/ctx_tracker.h"
#   include "hpy/runtime/ctx_tuple.h"
#   include "hpy/runtime/ctx_tuplebuilder.h"
#   include "hpy/cpython/misc.h"
#endif

#endif /* HPy_H */
