#include "hpy.h"

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
        switch (*fmt1++) {
        case 'l': {
            long *output = va_arg(vl, long *);
            long value = HPyLong_AsLong(ctx, args[i]);
            // XXX check for exceptions
            *output = value;
            break;
        }
        case 'O': {
            HPy *output = va_arg(vl, HPy *);
            *output = args[i];
            break;
        }
        default:
            abort();  // XXX
        }
        i++;
    }
    if (i != nargs) {
        abort();   // XXX
    }

    va_end(vl);
    return 1;
}

HPyAPI_RUNTIME_FUNC(int)
HPyArg_ParseKeywords(HPyContext ctx, HPy *args, HPy_ssize_t nargs, HPy kw, const char *fmt, char *keywords[], ...)
{
  va_list vl;
  va_start(vl, keywords);
  const char *fmt1 = fmt;
  HPy_ssize_t i = 0;

  while (*fmt1 != 0) {
      if (i >= nargs) {
          abort(); // XXX
      }
      switch (*fmt1++) {
      case 'l': {
          long *output = va_arg(vl, long *);
          long value = HPyLong_AsLong(ctx, args[i]);
          // XXX check for exceptions
          *output = value;
          break;
      }
      case 'O': {
          HPy *output = va_arg(vl, HPy *);
          *output = args[i];
          break;
      }
      default:
          abort();  // XXX
      }
      i++;
  }
  if (i != nargs) {
      abort();   // XXX
  }

  va_end(vl);
  return 1;
}
