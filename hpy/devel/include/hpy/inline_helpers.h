#ifndef HPY_INLINE_HELPERS_H
#define HPY_INLINE_HELPERS_H

#if defined(_MSC_VER)
# include <malloc.h>  /* for alloca() */
#endif

#include <assert.h>

/**
 * Same as :c:func:`HPyErr_SetFromErrnoWithFilenameObjects` but passes
 * ``HPy_NULL`` to the optional arguments.
 *
 * :param ctx:
 *     The execution context.
 * :param h_type:
 *     The exception type to raise.
 *
 * :return:
 *     always returns ``HPy_NULL``
 */
HPyAPI_INLINE_HELPER HPy
HPyErr_SetFromErrno(HPyContext *ctx, HPy h_type)
{
    return HPyErr_SetFromErrnoWithFilenameObjects(ctx, h_type, HPy_NULL, HPy_NULL);
}

/**
 * Same as :c:func:`HPyErr_SetFromErrnoWithFilenameObjects` but passes
 * ``HPy_NULL`` to the last (optional) argument.
 *
 * :param ctx:
 *     The execution context.
 * :param h_type:
 *     The exception type to raise.
 * :param filename:
 *     a filename; may be ``HPy_NULL``
 *
 * :return:
 *     always returns ``HPy_NULL``
 */
HPyAPI_INLINE_HELPER HPy
HPyErr_SetFromErrnoWithFilenameObject(HPyContext *ctx, HPy h_type, HPy filename)
{
    return HPyErr_SetFromErrnoWithFilenameObjects(ctx, h_type, filename, HPy_NULL);
}

/**
 * Create a tuple from arguments.
 *
 * This is just a convenience function that will allocate a temporary array of
 * ``HPy`` elements and use :c:func:`HPyTuple_FromArray` to create a tuple.
 *
 * :param ctx:
 *     The execution context.
 * :param n:
 *     The number of elements to pack into a tuple.
 * :param ...:
 *     Variable number of ``HPy`` arguments.
 *
 * :return:
 *     A new tuple with ``n`` elements or ``HPy_NULL`` in case of an error
 *     occurred.
 */
HPyAPI_INLINE_HELPER HPy
HPyTuple_Pack(HPyContext *ctx, HPy_ssize_t n, ...)
{
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

/**
 * Delete an attribute.
 *
 * This is the equivalent of the Python statement ``del o.attr_name``.
 *
 * :param ctx:
 *     The execution context.
 * :param obj:
 *     The object with the attribute.
 * :param name:
 *     The name (an unicode object) of the attribute.
 *
 * :return:
 *     ``0`` on success; ``-1`` in case of an error.
 */
HPyAPI_INLINE_HELPER int
HPy_DelAttr(HPyContext *ctx, HPy obj, HPy name)
{
    return HPy_SetAttr(ctx, obj, name, HPy_NULL);
}

/**
 * Delete an attribute.
 *
 * This is the equivalent of the Python statement ``del o.attr_name``.
 *
 * :param ctx:
 *     The execution context.
 * :param obj:
 *     The object with the attribute.
 * :param utf8_name:
 *     The name (an UTF-8 encoded C string) of the attribute.
 *
 * :return:
 *     ``0`` on success; ``-1`` in case of an error.
 */
HPyAPI_INLINE_HELPER int
HPy_DelAttr_s(HPyContext *ctx, HPy obj, const char *utf8_name)
{
    return HPy_SetAttr_s(ctx, obj, utf8_name, HPy_NULL);
}

/**
 * Create a Python long object from a C ``long`` value.
 *
 * :param ctx:
 *     The execution context.
 * :param l:
 *     A C long value.
 *
 * :return:
 *     A Python long object with the value of ``l`` or ``HPy_NULL`` on failure.
 */
HPyAPI_INLINE_HELPER HPy
HPyLong_FromLong(HPyContext *ctx, long l)
{
    if (sizeof(long) <= sizeof(int32_t))
        return HPyLong_FromInt32_t(ctx, (int32_t)l);
    assert(sizeof(long) <= sizeof(int64_t));
    return HPyLong_FromInt64_t(ctx, (int64_t)l);
}

/**
 * Create a Python long object from a C ``unsigned long`` value.
 *
 * :param ctx:
 *     The execution context.
 * :param l:
 *     A C ``unsigned long`` value.
 *
 * :return:
 *     A Python long object with the value of ``l`` or ``HPy_NULL`` on failure.
 */
HPyAPI_INLINE_HELPER HPy
HPyLong_FromUnsignedLong(HPyContext *ctx, unsigned long l)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return HPyLong_FromUInt32_t(ctx, (uint32_t)l);
    assert(sizeof(unsigned long) <= sizeof(uint64_t));
    return HPyLong_FromUInt64_t(ctx, (uint64_t)l);
}

/**
 * Create a Python long object from a C ``long long`` value.
 *
 * :param ctx:
 *     The execution context.
 * :param l:
 *     A C ``long long`` value.
 *
 * :return:
 *     A Python long object with the value of ``l`` or ``HPy_NULL`` on failure.
 */
HPyAPI_INLINE_HELPER HPy
HPyLong_FromLongLong(HPyContext *ctx, long long l)
{
    assert(sizeof(long long) <= sizeof(int64_t));
    return HPyLong_FromInt64_t(ctx, (int64_t)l);
}

/**
 * Create a Python long object from a C ``unsigned long long`` value.
 *
 * :param ctx:
 *     The execution context.
 * :param l:
 *     A C ``unsigned long long`` value.
 *
 * :return:
 *     A Python long object with the value of ``l`` or ``HPy_NULL`` on failure.
 */
HPyAPI_INLINE_HELPER HPy
HPyLong_FromUnsignedLongLong(HPyContext *ctx, unsigned long long l)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return HPyLong_FromUInt64_t(ctx, (uint64_t)l);
}

/**
 * Return a C ``long`` representation of the given Python long object. If the
 * object is not an instance of Python long, the object's ``__index__`` method
 * (if present) will be used to convert it to a Python long object.
 *
 * This function will raise an ``OverflowError`` if the value of the object is
 * out of range for a C ``long``.
 *
 * This function will raise a ``TypeError`` if:
 *
 * * The object is neither an instance of Python long nor it provides an
 *   ``__index__`` method.
 * * If the ``__index__`` method does not return an instance of Python long.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     Either an instance of Python long or an object that provides an
 *     ``__index__`` method (which returns a Python long).
 *
 * :return:
 *     A C ``long`` value. Errors will be indicated with return value ``-1``.
 *     In this case, use :c:func:`HPyErr_Occurred` to disambiguate.
 */
HPyAPI_INLINE_HELPER long
HPyLong_AsLong(HPyContext *ctx, HPy h)
{
    if (sizeof(long) <= sizeof(int32_t))
        return (long) HPyLong_AsInt32_t(ctx, h);
    else if (sizeof(long) <= sizeof(int64_t))
        return (long) HPyLong_AsInt64_t(ctx, h);
}

/**
 * Return a C ``unsigned long`` representation of the given Python long object.
 *
 * This function will raise a ``TypeError`` if the object is not an instance of
 * Python long and it will raise an ``OverflowError`` if the object's value is
 * negative or out of range for a C ``unsigned long``.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     The object to convert to C ``unsigned long`` (must be an instance of
 *     Python long).
 *
 * :return:
 *     A C ``unsigned long`` value. Errors will be indicated with return value
 *     ``(unsigned long)-1``. In this case, use :c:func:`HPyErr_Occurred` to
 *     disambiguate.
 */
HPyAPI_INLINE_HELPER unsigned long
HPyLong_AsUnsignedLong(HPyContext *ctx, HPy h)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return (unsigned long) HPyLong_AsUInt32_t(ctx, h);
    else if (sizeof(unsigned long) <= sizeof(uint64_t))
        return (unsigned long) HPyLong_AsUInt64_t(ctx, h);
}

/**
 * Return a C ``unsigned long`` representation of the given Python long object. If the
 * object is not an instance of Python long, the object's ``__index__`` method
 * (if present) will be used to convert it to a Python long object.
 *
 * If the object's value is out of range for an ``unsigned long``, return the
 * reduction of that value modulo ``ULONG_MAX + 1``. Therefore, this function
 * will **NOT** raise an ``OverflowError`` if the value of the object is out of
 * range for a C ``unsigned long``.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     Either an instance of Python long or an object that provides an
 *     ``__index__`` method (which returns a Python long).
 *
 * :return:
 *     A C ``unsigned long`` value. Errors will be indicated with return value
 *     ``(unsigned long)-1``. In this case, use :c:func:`HPyErr_Occurred` to
 *     disambiguate.
 */
HPyAPI_INLINE_HELPER unsigned long
HPyLong_AsUnsignedLongMask(HPyContext *ctx, HPy h)
{
    if (sizeof(unsigned long) <= sizeof(uint32_t))
        return (unsigned long) HPyLong_AsUInt32_tMask(ctx, h);
    else if (sizeof(unsigned long) <= sizeof(uint64_t))
        return (unsigned long) HPyLong_AsUInt64_tMask(ctx, h);
}

/**
 * Return a C ``long long`` representation of the given Python long object. If
 * the object is not an instance of Python long, the object's ``__index__``
 * method (if present) will be used to convert it to a Python long object.
 *
 * This function will raise an ``OverflowError`` if the value of the object is
 * out of range for a C ``long long``.
 *
 * This function will raise a ``TypeError`` if:
 *
 * * The object is neither an instance of Python long nor it provides an
 *   ``__index__`` method.
 * * If the ``__index__`` method does not return an instance of Python long.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     Either an instance of Python long or an object that provides an
 *     ``__index__`` method (which returns a Python long).
 *
 * :return:
 *     A C ``long long`` value. Errors will be indicated with return value
 *     ``-1``. In this case, use :c:func:`HPyErr_Occurred` to disambiguate.
 */
HPyAPI_INLINE_HELPER long long
HPyLong_AsLongLong(HPyContext *ctx, HPy h)
{
    assert(sizeof(long long) <= sizeof(int64_t));
    return (long long) HPyLong_AsInt64_t(ctx, h);
}

/**
 * Return a C ``unsigned long long`` representation of the given Python long
 * object.
 *
 * This function will raise a ``TypeError`` if the object is not an instance of
 * Python long and it will raise an ``OverflowError`` if the object's value is
 * negative or out of range for a C ``unsigned long``.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     The object to convert to C ``unsigned long long`` (must be an instance of
 *     Python long).
 *
 * :return:
 *     A C ``unsigned long long`` value. Errors will be indicated with return
 *     value ``(unsigned long long)-1``. In this case, use
 *     :c:func:`HPyErr_Occurred` to disambiguate.
 */
HPyAPI_INLINE_HELPER unsigned long long
HPyLong_AsUnsignedLongLong(HPyContext *ctx, HPy h)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return (unsigned long long) HPyLong_AsUInt64_t(ctx, h);
}

/**
 * Return a C ``unsigned long long`` representation of the given Python long
 * object. If the object is not an instance of Python long, the object's
 * ``__index__`` method (if present) will be used to convert it to a Python long
 * object.
 *
 * If the object's value is out of range for an ``unsigned long long``, return
 * the reduction of that value modulo ``ULLONG_MAX + 1``. Therefore, this
 * function will **NOT** raise an ``OverflowError`` if the value of the object
 * is out of range for a C ``unsigned long long``.
 *
 * :param ctx:
 *     The execution context.
 * :param h:
 *     Either an instance of Python long or an object that provides an
 *     ``__index__`` method (which returns a Python long).
 *
 * :return:
 *     A C ``unsigned long`` value. Errors will be indicated with return value
 *     ``(unsigned long long)-1``. In this case, use :c:func:`HPyErr_Occurred`
 *     to disambiguate.
 */
HPyAPI_INLINE_HELPER unsigned long long
HPyLong_AsUnsignedLongLongMask(HPyContext *ctx, HPy h)
{
    assert(sizeof(unsigned long long) <= sizeof(uint64_t));
    return (unsigned long long) HPyLong_AsUInt64_tMask(ctx, h);
}

/**
 * Returns Python ``true`` or ``false`` depending on the truth value of ``v``.
 *
 * :param ctx:
 *     The execution context.
 * :param v:
 *     A C ``long`` value.
 *
 * :return:
 *     Python ``true`` if ``v != 0``; Python ``false`` otherwise.
 */
HPyAPI_INLINE_HELPER HPy
HPyBool_FromLong(HPyContext *ctx, long v)
{
    return HPyBool_FromBool(ctx, (v ? true : false));
}

#endif //HPY_INLINE_HELPERS_H
