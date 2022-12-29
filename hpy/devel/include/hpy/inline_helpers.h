#ifndef HPY_INLINE_HELPERS_H
#define HPY_INLINE_HELPERS_H

#if defined(_MSC_VER)
# include <malloc.h>  /* for alloca() */
#endif

#include <assert.h>

HPyAPI_FUNC HPy HPyErr_SetFromErrno(HPyContext *ctx, HPy h_type)
{
    return HPyErr_SetFromErrnoWithFilenameObjects(ctx, h_type, HPy_NULL, HPy_NULL);
}

HPyAPI_FUNC HPy HPyErr_SetFromErrnoWithFilenameObject(HPyContext *ctx, HPy h_type, HPy filename)
{
    return HPyErr_SetFromErrnoWithFilenameObjects(ctx, h_type, filename, HPy_NULL);
}

HPyAPI_FUNC HPy HPyTuple_Pack(HPyContext *ctx, HPy_ssize_t n, ...) {
    va_list vargs;
    HPy_ssize_t i;

    if (n == 0) {
        return HPyTuple_FromArray(ctx, (HPy*)NULL, n);
    }
    HPy *array = (HPy *)alloca(n * sizeof(HPy));
    va_start(vargs, n);
    if (array == NULL) {
        va_end(vargs);
        return HPy_NULL;
    }
    for (i = 0; i < n; i++) {
        array[i] = va_arg(vargs, HPy);
    }
    va_end(vargs);
    return HPyTuple_FromArray(ctx, array, n);
}

HPyAPI_FUNC int HPy_DelAttr(HPyContext *ctx, HPy obj, HPy name) {
    return HPy_SetAttr(ctx, obj, name, HPy_NULL);
}

HPyAPI_FUNC int HPy_DelAttr_s(HPyContext *ctx, HPy obj, const char *utf8_name) {
    return HPy_SetAttr_s(ctx, obj, utf8_name, HPy_NULL);
}

HPyAPI_FUNC HPy
HPyLong_FromLong(HPyContext *ctx, long l)
{
    if (sizeof(long) <= sizeof(int32_t))
        return HPyLong_FromInt32_t(ctx, (int32_t)l);
    assert(sizeof(long) <= sizeof(int64_t));
    return HPyLong_FromInt64_t(ctx, (int64_t)l);
}

HPyAPI_FUNC HPy
HPyLong_FromUnsignedLong(HPyContext *ctx, unsigned long l)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return HPyLong_FromUInt32_t(ctx, (uint32_t)l);
    assert(sizeof(unsigned long) <= sizeof(uint64_t));
    return HPyLong_FromUInt64_t(ctx, (uint64_t)l);
}

HPyAPI_FUNC HPy
HPyLong_FromLongLong(HPyContext *ctx, long long l)
{
    assert(sizeof(long long) <= sizeof(int64_t));
    return HPyLong_FromInt64_t(ctx, (int64_t)l);
}

HPyAPI_FUNC HPy
HPyLong_FromUnsignedLongLong(HPyContext *ctx, unsigned long long l)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return HPyLong_FromUInt64_t(ctx, (uint64_t)l);
}

HPyAPI_FUNC long
HPyLong_AsLong(HPyContext *ctx, HPy h)
{
    if (sizeof(long) <= sizeof(int32_t))
        return (long) HPyLong_AsInt32_t(ctx, h);
    else if (sizeof(long) <= sizeof(int64_t))
        return (long) HPyLong_AsInt64_t(ctx, h);
}

HPyAPI_FUNC unsigned long
HPyLong_AsUnsignedLong(HPyContext *ctx, HPy h)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return (unsigned long) HPyLong_AsUInt32_t(ctx, h);
    else if (sizeof(unsigned long) <= sizeof(uint64_t))
        return (unsigned long) HPyLong_AsUInt64_t(ctx, h);
}

HPyAPI_FUNC unsigned long
HPyLong_AsUnsignedLongMask(HPyContext *ctx, HPy h)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return (unsigned long) HPyLong_AsUInt32_tMask(ctx, h);
    else if (sizeof(unsigned long) <= sizeof(uint64_t))
        return (unsigned long) HPyLong_AsUInt64_tMask(ctx, h);
}

HPyAPI_FUNC long long
HPyLong_AsLongLong(HPyContext *ctx, HPy h)
{
    assert(sizeof(long long) <= sizeof(int64_t));
    return (long long) HPyLong_AsInt64_t(ctx, h);
}

HPyAPI_FUNC unsigned long long
HPyLong_AsUnsignedLongLong(HPyContext *ctx, HPy h)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return (unsigned long long) HPyLong_AsUInt64_t(ctx, h);
}

HPyAPI_FUNC unsigned long long
HPyLong_AsUnsignedLongLongMask(HPyContext *ctx, HPy h)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return (unsigned long long) HPyLong_AsUInt64_tMask(ctx, h);
}

#endif //HPY_INLINE_HELPERS_H
