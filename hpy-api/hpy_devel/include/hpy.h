#ifndef HPy_H
#define HPy_H

#ifdef HPY_UNIVERSAL_ABI
#    include "universal/hpy.h"
#else
/*
 *   By default, limit the CPython C-API. If you wish to use the
 *   extended API, you must #include "Python.h" before #including this file
 */
#    ifndef Py_PYTHON_H
#    define Py_LIMITED_API
#    endif
#    include "cpython/hpy.h"
#endif

#endif /* HPy_H */
