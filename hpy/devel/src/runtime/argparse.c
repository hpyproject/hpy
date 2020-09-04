#include "hpy.h"

#define _BREAK_IF_OPTIONAL(current_arg) if (HPy_IsNull(current_arg)) break;

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

static void
skip_next_format_character(HPyContext ctx, const char **fmt, va_list *vl)
{
    if (!parse_item(ctx, HPy_NULL, fmt, vl))
        HPy_FatalError(ctx, "argparse.c: skip_next_format_character failed");
}

static void
clear_already_allocated_handles(HPyContext ctx, const char *fmt,
                                const char *fmt_end, va_list *vl)
{
    while (fmt != fmt_end) {
        if (*fmt == '|' || *fmt == '$') {
          fmt++;
          continue;
        }
        if (*fmt == 'N') {
            /* if the current code in 'fmt' is 'N', then close the handle
               previously written there */
            HPy *output = va_arg(*vl, HPy *);
            fmt++;
            HPy_Close(ctx, *output);
            *output = HPy_NULL;
        }
        else {
            /* else, move 'vl' forward past other 'fmt' characters */
            skip_next_format_character(ctx, &fmt, vl);
        }
    }
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_Parse(HPyContext ctx, HPy *args, HPy_ssize_t nargs, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    const char *fmt1 = fmt;
    const char *fmt_end = fmt;
    int optional = 0;
    HPy_ssize_t i = 0;

    while (*fmt1 != 0) {
        if (*fmt1 == '|') {
            optional = 1;
            fmt1++;
            continue;
        }
        if (i < nargs) {
            HPy current_arg = args[i];
            if (!parse_item(ctx, current_arg, &fmt1, &vl))
                goto error;
        }
        else if (optional) {
            skip_next_format_character(ctx, &fmt1, &vl);
        }
        else {
            HPyErr_SetString(ctx, ctx->h_TypeError,
                             "XXX: required positional argument missing");
            goto error;
        }
        fmt_end = fmt1;
        i++;
    }
    if (i < nargs) {
        HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: mismatched args (too many arguments for fmt)");
        va_end(vl);
        return 0;
    }

    va_end(vl);
    return 1;

  error:
    va_end(vl);

    va_start(vl, fmt);
    clear_already_allocated_handles(ctx, fmt, fmt_end, &vl);
    va_end(vl);
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_ParseKeywords(HPyContext ctx, HPy *args, HPy_ssize_t nargs, HPy kw,
                     const char *fmt, const char *keywords[], ...)
{
    va_list vl;
    const char *fmt1 = fmt;
    const char *fmt_end = fmt;
    int optional = 0;
    int keyword_only = 0;
    HPy_ssize_t i = 0;
    HPy_ssize_t nkw = 0;

    // first count positional only arguments
    while (keywords[nkw] != NULL && !*keywords[nkw])
        nkw++;
    // then check and count the rest
    while (keywords[nkw] != NULL) {
        if (!*keywords[nkw]) {
            HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: Empty keyword parameter name");
            return 0;
        }
        nkw++;
    }

    va_start(vl, keywords);

    while (*fmt1 != 0) {
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
            HPyErr_SetString(ctx, ctx->h_ValueError,
                "XXX: HPyArg_ParseKeywords() cannot use the format character "
                "'O'. Use 'N' instead, and close the returned handle if the "
                "call returns successfully");
            goto error;
        }
        if (i >= nkw) {
            HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: mismatched args (too few keywords for fmt)");
            goto error;
        }
        if (i < nargs) {
            if (keyword_only) {
                HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: keyword only argument passed as positional argument");
                goto error;
            }
            HPy current_arg = args[i];
            if (!parse_item(ctx, current_arg, &fmt1, &vl))
                goto error;
        }
        else {
            HPy current_arg;
            if (!HPy_IsNull(kw) && *keywords[i])
                current_arg = HPy_GetItem_s(ctx, kw, keywords[i]);
            else
                current_arg = HPy_NULL;

            if (!HPy_IsNull(current_arg)) {
                int ok = parse_item(ctx, current_arg, &fmt1, &vl);
                HPy_Close(ctx, current_arg);
                if (!ok)
                    goto error;
            }
            else {
                HPyErr_Clear(ctx);
                if (optional) {
                    skip_next_format_character(ctx, &fmt1, &vl);
                }
                else {
                    HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: no value for required argument");
                    goto error;
                }
            }
        }
        fmt_end = fmt1;
        i++;
    }
    if (i != nkw) {
        HPyErr_SetString(ctx, ctx->h_TypeError, "XXX: mismatched args (too many keywords for fmt)");
        goto error;
    }

    va_end(vl);
    return 1;

  error:
    va_end(vl);

    va_start(vl, keywords);
    clear_already_allocated_handles(ctx, fmt, fmt_end, &vl);
    va_end(vl);
    return 0;
}
