#include "hpy.h"

int _HPyArg_ParseItem(HPyContext ctx, HPy current_arg, const char **fmt, va_list vl)
{
  switch (*(*fmt)++) {
  case 'l': {
      long *output = va_arg(vl, long *);
      long value = HPyLong_AsLong(ctx, current_arg);
      // XXX check for exceptions
      *output = value;
      break;
  }
  case 'O': {
      HPy *output = va_arg(vl, HPy *);
      *output = current_arg;
      break;
  }
  default:
      abort();  // XXX
  }
  return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_Parse(HPyContext ctx, HPy *args, HPy_ssize_t nargs, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    const char *fmt1 = fmt;
    HPy_ssize_t i = 0;

    while (*fmt1 != 0) {
        if (i >= nargs) {
            abort(); // XXX
        }
        _HPyArg_ParseItem(ctx, args[i], &fmt1, vl);
        i++;
    }
    if (i != nargs) {
        abort();   // XXX
    }

    va_end(vl);
    return 1;
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_ParseKeywords(HPyContext ctx, HPy *args, HPy_ssize_t nargs, HPy kw,
                     const char *fmt, const char *keywords[], ...)
{
  va_list vl;
  va_start(vl, keywords);
  const char *fmt1 = fmt;
  int optional = 0;
  HPy_ssize_t i = 0;
  HPy_ssize_t nkw = 0;
  HPy current_arg;

  while (keywords[nkw] != NULL) nkw++;

  while (*fmt1 != 0) {
      if (*fmt1 == '|') {
        optional = 1;
        fmt1++;
        continue;
      }
      if (i >= nkw) {
        abort(); // XXX
      }
      if (i < nargs) {
        current_arg = args[i];
      }
      else {
        current_arg = HPyDict_GetItem(ctx, kw, HPyUnicode_FromString(ctx, keywords[i]));
      }
      if (!HPy_IsNull(current_arg)) {
        _HPyArg_ParseItem(ctx, current_arg, &fmt1, vl);
      }
      else if (optional) {
        fmt1++;
      }
      else {
        abort(); // XXX
      }
      i++;
  }
  if (i != nkw) {
      abort();   // XXX
  }

  va_end(vl);
  return 1;
}
