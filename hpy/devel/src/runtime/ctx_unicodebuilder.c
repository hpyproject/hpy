#include <stddef.h>
#include <string.h>
#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

static const Py_ssize_t HPYUNICODEBUILDER_INITIAL_CAPACITY = 1024;

typedef struct {
    Py_ssize_t capacity;  // allocated handles
    Py_ssize_t length;
    char *buf;
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
        // TODO: default value or raise a ValueError?
        capacity = HPYUNICODEBUILDER_INITIAL_CAPACITY;
    }
    capacity++; // always reserve space for the trailing 0

    bp = malloc(sizeof(_PyUnicodeBuilder_s));
    if (bp == NULL) {
        HPyErr_NoMemory(ctx);
        return _pb2hb(0);
    }

    bp->buf = calloc(1, capacity);
    if (bp == NULL) {
        free(bp);
        HPyErr_NoMemory(ctx);
        return _pb2hb(0);
    }

    bp->capacity = capacity;
    bp->length = 0;
    return _pb2hb(bp);
}

_HPy_HIDDEN int
ctx_UnicodeBuilder_Add(HPyContext *ctx, HPyUnicodeBuilder builder, const char *item)
{
    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    // TODO: Should we trust the user to submit a 0 terminated string?
    // The alternative would be to use strnlen and have a maximum allowed length for s
    int len = strlen(item);
    if(bp->length + len >= bp->capacity) {
        // TODO: Have a better reallocation strategy
        int new_size = bp->capacity + len + 1;
        bp->buf = realloc(bp->buf, new_size);
        if(bp->buf == NULL) {
            free(bp);
            HPyErr_NoMemory(ctx);
            return -1;
        }
    }
    strncpy((bp->buf + bp->length), item, (bp->capacity - bp->length));
    bp->length += len;
    return 0;
}

_HPy_HIDDEN HPy
ctx_UnicodeBuilder_Build(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    HPy h_result = HPyUnicode_FromString(ctx, bp->buf);
    free(bp->buf);
    free(bp);
    return h_result;
}

_HPy_HIDDEN void
ctx_UnicodeBuilder_Cancel(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    _PyUnicodeBuilder_s *bp = _hb2pb(builder);
    free(bp->buf);
    free(bp);
}
