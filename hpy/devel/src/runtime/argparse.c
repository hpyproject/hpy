/**
 * Implementation of HPyArg_Parse and HPyArg_ParseKeywords.
 *
 * HPyArg_Parse parses positional arguments and replaces PyArg_ParseTuple.
 * HPyArg_ParseKeywords parses positional and keyword arguments and
 * replaces PyArg_ParseTupleAndKeywords.
 *
 * HPy intends to only support the simpler format string types (numbers, bools)
 * and handles. More complex types (e.g. buffers) should be retrieved as
 * handles and then processed further as needed.
 *
 * Supported formatting strings:
 *
 * Numbers
 * -------
 *
 * i (int) [int]
 *     Convert a Python integer to a plain C int.
 *
 * l (int) [long int]
 *     Convert a Python integer to a C long int.
 *
 * d (float) [double]
 *     Convert a Python floating point number to a C double.
 *
 * Handles
 * -------
 *
 * O (object) [HPy *]
 *     Returns an existing handle. Not supported by HPyArg_ParseKeywords as
 *     retrieving an item from the keywords dictionary would require creating
 *     a new handled. Use O+ instead for this case.
 *
 * O+ (object) [HPy *]
 *     Returns a new handle. The new handle must be closed if the argument
 *     parsing returns successfully.
 *
 * Options
 * -------
 *
 * |
 *     Indicates that the remaining arguments in the argument list are optional.
 *     The C variables corresponding to optional arguments should be initialized
 *     to their default value — when an optional argument is not specified, the
 *     contents of the corresponding C variable is not modified.
 *
 * $
 *     HPyArg_ParseKeywords() only: Indicates that the remaining arguments in
 *     the argument list are keyword-only. Currently, all keyword-only arguments
 *     must also be optional arguments, so | must always be specified before $
 *     in the format string.
 *
 * :
 *     The list of format units ends here; the string after the colon is used as
 *     the function name in error messages. : and ; are mutually exclusive and
 *     whichever occurs first takes precedence.
 *
 * ;
 *     The list of format units ends here; the string after the semicolon is
 *     used as the error message instead of the default error message. : and ;
 *     are mutually exclusive and whichever occurs first takes precedence.
 *
 */

#include <stdio.h>
#include "hpy.h"

#define _BREAK_IF_OPTIONAL(current_arg) if (HPy_IsNull(current_arg)) break;
#define _MAX_ERR_STRING_LENGTH 512

static int
parse_item(HPyContext ctx, HPy current_arg, const char **fmt, va_list *vl)
{
    switch (*(*fmt)++) {
    case 'i': {
        int *output = va_arg(*vl, int *);
        _BREAK_IF_OPTIONAL(current_arg);
        long value = HPyLong_AsLong(ctx, current_arg);
        if (value == -1 && HPyErr_Occurred(ctx))
            return 0;
        *output = (int)value;
        break;
    }
    case 'l': {
        long *output = va_arg(*vl, long *);
        _BREAK_IF_OPTIONAL(current_arg);
        long value = HPyLong_AsLong(ctx, current_arg);
        if (value == -1 && HPyErr_Occurred(ctx))
            return 0;
        *output = value;
        break;
    }
    case 'd': {
        double* output = va_arg(*vl, double *);
        _BREAK_IF_OPTIONAL(current_arg);
        double value = HPyFloat_AsDouble(ctx, current_arg);
        if (value == -1.0 && HPyErr_Occurred(ctx))
            return 0;
        *output = value;
        break;
    }
    case 'O': {
        HPy *output = va_arg(*vl, HPy *);
        _BREAK_IF_OPTIONAL(current_arg);
        *output = current_arg;
        break;
    }
    case 'N': {
        HPy *output = va_arg(*vl, HPy *);
        _BREAK_IF_OPTIONAL(current_arg);
        *output = HPy_Dup(ctx, current_arg);
        if (HPy_IsNull(*output))
            return 0;
        break;
    }
    default:
        HPyErr_SetString(ctx, ctx->h_ValueError, "XXX: Unknown arg format code");
        return 0;
    }
    return 1;
}

static const char *
parse_errstring(const char *fmt, const char **err_funcname, const char **err_message)
{
    const char *fmt1 = fmt;

    for (fmt1 = fmt; *fmt1 != 0; fmt1++) {
        if (*fmt1 == ':') {
            *err_funcname = fmt1 + 1;
            break;
        }
        if (*fmt1 == ';') {
            *err_message = fmt1 + 1;
            break;
        }
    }
    return fmt1;
}

static void
set_error(HPyContext ctx, HPy exc, const char *err_funcname, const char *err_message, const char *msg) {
    char err_buf[_MAX_ERR_STRING_LENGTH];
    if (err_message != NULL) {
        snprintf(err_buf, _MAX_ERR_STRING_LENGTH, "%s", err_message);
    }
    if (err_message == NULL) {
        if (err_funcname == NULL) {
            snprintf(err_buf, _MAX_ERR_STRING_LENGTH, "function %.256s", msg);
        }
        else {
            snprintf(err_buf, _MAX_ERR_STRING_LENGTH, "%.200s() %.256s", err_funcname, msg);
            err_funcname = "XXX";
        }
    }
    HPyErr_SetString(ctx, exc, err_buf);
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_Parse(HPyContext ctx, HPy *args, HPy_ssize_t nargs, const char *fmt, ...)
{
    const char *fmt1 = fmt;
    const char *err_funcname = NULL;
    const char *err_message = NULL;
    const char *fmt_end = NULL;

    int optional = 0;
    HPy_ssize_t i = 0;
    HPy current_arg;

    fmt_end = parse_errstring(fmt, &err_funcname, &err_message);

    va_list vl;
    va_start(vl, fmt);

    while (fmt1 != fmt_end) {
        if (*fmt1 == '|') {
            optional = 1;
            fmt1++;
            continue;
        }
        current_arg = HPy_NULL;
        if (i < nargs) {
            current_arg = args[i];
        }
        if (!HPy_IsNull(current_arg) || optional) {
            if (!parse_item(ctx, current_arg, &fmt1, &vl)) {
                va_end(vl);
                return 0;
            }
        }
        else {
            set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
                "required positional argument missing");
            va_end(vl);
            return 0;
        }
        i++;
    }
    if (i < nargs) {
        set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
            "mismatched args (too many arguments for fmt)");
        va_end(vl);
        return 0;
    }

    va_end(vl);
    return 1;
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_ParseKeywords(HPyContext ctx, HPy *args, HPy_ssize_t nargs, HPy kw,
                     const char *fmt, const char *keywords[], ...)
{
    const char *fmt1 = fmt;
    const char *err_funcname = NULL;
    const char *err_message = NULL;
    const char *fmt_end = NULL;

    int optional = 0;
    int keyword_only = 0;
    HPy_ssize_t i = 0;
    HPy_ssize_t nkw = 0;
    HPy current_arg;

    fmt_end = parse_errstring(fmt, &err_funcname, &err_message);

    // first count positional only arguments
    while (keywords[nkw] != NULL && !*keywords[nkw]) nkw++;
    // then check and count the rest
    while (keywords[nkw] != NULL) {
        if (!*keywords[nkw]) {
            set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
                "Empty keyword parameter name");
            return 0;
        }
        nkw++;
    }

    va_list vl;
    va_start(vl, keywords);

    while (fmt1 != fmt_end) {
        if (*fmt1 == '|') {
            optional = 1;
            fmt1++;
            continue;
        }
        if (*fmt1 == '$') {
            optional = 1;
            keyword_only = 1;
            fmt1++;
            continue;
        }
        if (*fmt1 == 'O') {
            set_error(ctx, ctx->h_ValueError, err_funcname, err_message,
                "HPyArg_ParseKeywords cannot use the format character 'O'."
                " Use 'N' instead and close the the returned handle if the call"
                " returns successfully");
        }
        if (i >= nkw) {
            set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
                "mismatched args (too few keywords for fmt)");
            va_end(vl);
            return 0;
        }
        current_arg = HPy_NULL;
        if (i < nargs) {
            if (keyword_only) {
                set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
                    "keyword only argument passed as positional argument");
                va_end(vl);
                return 0;
            }
            current_arg = args[i];
        }
        else if (!HPy_IsNull(kw) && *keywords[i]) {
            current_arg = HPy_GetItem_s(ctx, kw, keywords[i]);
        }
        if (!HPy_IsNull(current_arg) || optional) {
            if (!parse_item(ctx, current_arg, &fmt1, &vl)) {
                va_end(vl);
                return 0;
            }
        }
        else {
            set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
                "no value for required argument");
            va_end(vl);
            return 0;
        }
        i++;
    }
    if (i != nkw) {
        set_error(ctx, ctx->h_TypeError, err_funcname, err_message,
            "mismatched args (too many keywords for fmt)");
        va_end(vl);
        return 0;
    }

    va_end(vl);
    return 1;
}
