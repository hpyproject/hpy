#ifndef HPY_UNIVERSAL_METH_H
#define HPY_UNIVERSAL_METH_H

#include "hpyfunc.h"

typedef struct {
    int slot;
    void *impl;                     // function pointer to the implementation
    void *cpy_trampoline;           // used by CPython to call impl
    HPyFunc_Signature signature;    // Indicates impl's expected the signature

    // the following fields are used only if slot == HPy_meth
    const char *name;               // The name of the built-in function/method
    const char *doc;                // The __doc__ attribute, or NULL
} HPyMeth;




/* Macro to define an HPyMeth:
 *     - declare the expected prototype for impl
 *     - create the corresponding CPython trampoline
 *     - define HPyMeth and fill all the fields
 */

#define HPyDef_METH(SYM, NAME, IMPL, SIG)                            \
    HPyFunc_DECLARE(IMPL, SIG);                                         \
    HPyFunc_TRAMPOLINE(SYM##_trampoline, IMPL, SIG);                    \
    HPyMeth SYM = {                                                     \
        .slot = HPy_meth,                                               \
        .impl = IMPL,                                                   \
        .cpy_trampoline = SYM##_trampoline,                             \
        .signature = SIG,                                               \
        .name = NAME                                                    \
    };

#endif // HPY_UNIVERSAL_METH_H
