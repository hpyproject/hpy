/**
 * Implementation of HPy_BuildValue.
 *
 * HPy_BuildValue creates a new value based on a format string from the values
 * passed in an array. Returns HPy_NULL in case of an error and raises an exception.
 *
 * HPy_BuildValue does not always build a tuple. It builds a tuple only if its format
 * string contains two or more format units. If the format string is empty, it returns
 * None; if it contains exactly one format unit, it returns whatever object is described
 * by that format unit. To force it to return a tuple of size 0 or one, parenthesize the
 * format string.
 *
 * Building complex values with HPy_BuildValue is more convenient than the equivalent
 * code that uses more granular APIs with proper error handling and cleanup. Moreover,
 * HPy_BuildValue provides straightforward way to port existing code that uses
 * Py_BuildValue.
 *
 * Supported Formatting Strings
 * ----------------------------
 *
 * Numbers
 * ~~~~~~~
 *
 * ``i (int) [int]``
 *     Convert a plain C int to a Python integer object.
 *
 * ``f (float) [float]``
 *     Convert a C float to a Python floating point number.
 *
 * Collections
 * ~~~~~~~
 *
 * ``(items) (tuple) [matching-items]``
 *     Convert a sequence of C values to a Python tuple with the same number of items.
 *
 * API
 * ---
 *
 */

#include "hpy.h"
#include <stdarg.h>

static HPy_ssize_t count_items(HPyContext *ctx, const char *fmt, char end);
static HPy build_tuple(HPyContext *ctx, const char **fmt, va_list *values, HPy_ssize_t size, char expected_end);
static HPy build_single(HPyContext *ctx, const char **fmt, va_list *values);

// HPyAPI_HELPER
HPy HPy_BuildValue(HPyContext *ctx, const char *fmt, ...)
{
    va_list values;
    va_start(values, fmt);
    HPy_ssize_t size = count_items(ctx, fmt, '\0');
    if (size < 0) {
        return HPy_NULL;
    } else if (size == 1) {
        return build_single(ctx, &fmt, &values);
    } else {
        return build_tuple(ctx, &fmt, &values, size, '\0');
    }
}

static HPy_ssize_t count_items(HPyContext *ctx, const char *fmt, char end)
{
    HPy_ssize_t level = 0, result = 0;
    while (*fmt != end) {
        switch (*fmt++) {
            case '\0':
                // Premature end
                HPyErr_SetString(ctx, ctx->h_SystemError, "unmatched paren in format");
                return -1;

            case '[':
            case '(':
            case '{':
                if (level == 0) {
                    result++;
                }
                level++;
                break;

            case ']':
            case ')':
            case '}':
                level--;
                break;

            case ' ':
                break;

            default:
                if (level == 0) {
                    result++;
                }
        }
    }
    return result;
}

static HPy build_single(HPyContext *ctx, const char **fmt, va_list *values)
{
    switch (*(*fmt)++) {
        case '(': {
            HPy_ssize_t size = count_items(ctx, *fmt, ')');
            return build_tuple(ctx, fmt, values, size, ')');
        }

        case 'i':
            return HPyLong_FromLong(ctx, (long)va_arg(*values, int));

        case 'f':
            return HPyFloat_FromDouble(ctx, (double)va_arg(*values, double));

        default: {
            HPyErr_SetString(ctx, ctx->h_SystemError, "NULL object passed to Py_BuildValue");
            return HPy_NULL;
        }
    } // switch
}

static HPy build_tuple(HPyContext *ctx, const char **fmt, va_list *values, HPy_ssize_t size, char expected_end)
{
    HPyTupleBuilder builder = HPyTupleBuilder_New(ctx, size);
    for (HPy_ssize_t i = 0; i < size; ++i) {
        HPy item = build_single(ctx, fmt, values);
        if (HPy_IsNull(item)) {
            HPyTupleBuilder_Cancel(ctx, builder);
            return HPy_NULL;
        }
        HPyTupleBuilder_Set(ctx, builder, i, item);
        HPy_Close(ctx, item);
    }
    if (**fmt != expected_end) {
        HPyTupleBuilder_Cancel(ctx, builder);
        HPyErr_SetString(ctx, ctx->h_SystemError, "Unmatched paren in format");
        return HPy_NULL;
    }
    if (expected_end != '\0') {
        ++*fmt;
    }
    return HPyTupleBuilder_Build(ctx, builder);
}