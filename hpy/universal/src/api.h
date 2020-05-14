#ifndef HPY_API_H
#define HPY_API_H

#include "universal/hpy.h"

#define HPyAPI_STORAGE _HPy_HIDDEN

#define CONSTANT_H_NULL                0
#define CONSTANT_H_NONE                1
#define CONSTANT_H_TRUE                2
#define CONSTANT_H_FALSE               3
#define CONSTANT_H_VALUEERROR          4
#define CONSTANT_H_TYPEERROR           5
#define CONSTANT_H__TOTAL              6

extern struct _HPyContext_s global_ctx;
HPy _py2h(PyObject *);
PyObject *_h2py(HPy);
void _hclose(HPy);


/* declare alloca() */
#if defined(_MSC_VER)
# include <malloc.h>   /* for alloca() */
#else
# include <stdint.h>
# if (defined (__SVR4) && defined (__sun)) || defined(_AIX) || defined(__hpux)
#  include <alloca.h>
# endif
#endif



#endif /* HPY_API_H */
