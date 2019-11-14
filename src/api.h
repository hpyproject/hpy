#ifndef HPY_API_H
#define HPY_API_H

#include "universal/hpy.h"

extern struct _HPyContext_s global_ctx;
HPy _py2h(PyObject *);
PyObject *_h2py(HPy);
void _hclose(HPy);

#endif /* HPY_API_H */
