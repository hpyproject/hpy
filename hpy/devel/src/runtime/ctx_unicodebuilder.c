#include <stddef.h>
#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

static const Py_ssize_t HPYUNICODEBUILDER_INITIAL_CAPACITY = 5;

typedef struct {
    Py_ssize_t capacity;  // allocated handles
    Py_ssize_t length;    // used handles
    PyObject *list;
} _PyUnicodeBuilder_s;

#ifdef HPY_UNIVERSAL_ABI
static inline _PyUnicodeBuilder_s *_hb2pb(HPyUnicodeBuilder ht) {
    return (_PyUnicodeBuilder_s *) (ht)._i;
}
static inline HPyUnicodeBuilder _pb2hb(_PyUnicodeBuilder_s *bp) {
    return (HPyUnicodeBuilder) {(HPy_ssize_t) (bp)};
}
#else
static inline _PyUnicodeBuilder_s *_hb2pb(HPyUnicodeBuilder ht) {
    return (_PyUnicodeBuilder_s *) (ht)._o;
}
static inline HPyUnicodeBuilder _pb2hb(_PyUnicodeBuilder_s *bp) {
    return (HPyUnicodeBuilder) {(void *) (bp)};
}
#endif

_HPy_HIDDEN HPyUnicodeBuilder
ctx_UnicodeBuilder_New(HPyContext *ctx, HPy_ssize_t capacity)
{
    _PyUnicodeBuilder_s *bp;
    if (capacity == 0) {
        capacity = HPYUNICODEBUILDER_INITIAL_CAPACITY;
    }
    capacity++; // always reserve space for an extra handle, see the docs, analogue to HPyTracker

    bp = malloc(sizeof(_PyUnicodeBuilder_s));
    if (bp == NULL) {
        HPyErr_NoMemory(ctx);
        return _pb2hb(0);
    }

    bp->list = PyList_New(capacity);
    if (bp->list == NULL) {
        free(bp);
        HPyErr_NoMemory(ctx);
        return _pb2hb(0);
    }
    bp->capacity = capacity;
    bp->length = 0;
    return _pb2hb(bp);
}

_HPy_HIDDEN int
ctx_UnicodeBuilder_Add(HPyContext *ctx, HPyUnicodeBuilder builder, HPy h_item)
{
    if(!HPyUnicode_Check(ctx, h_item)) {
        HPyErr_SetString(ctx, ctx->h_TypeError, "Argument must be of type HPyUnicode");
        return -1;
    }

    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    PyObject *item = _h2py(h_item);

    // XXX: For the initial PoC we don't care about reallocation
    if (bp->capacity <= bp->length) {
        return -1;
    }
    Py_INCREF(item);
    PyList_SET_ITEM(bp->list, bp->length++, item);
    return 0;
}

_HPy_HIDDEN HPy
ctx_UnicodeBuilder_Build(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    PyObject *list = PyList_GetSlice(bp->list, 0, bp->length);

    PyObject *sep = PyUnicode_FromString("");
    PyObject *str = PyUnicode_Join(sep, list);
    Py_XDECREF(sep);

    if(str == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }

    Py_XDECREF(bp->list);
    Py_XDECREF(list);
    free(bp);
    return _py2h(str);
}

_HPy_HIDDEN void
ctx_UnicodeBuilder_Cancel(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    Py_XDECREF(bp->list);
    free(bp);
}
