#ifndef HPy_H
#define HPy_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __GNUC__
#define _HPy_HIDDEN  __attribute__((visibility("hidden")))
#else
#define _HPy_HIDDEN
#endif /* __GNUC__ */

#if defined(__clang__) || \
    (defined(__GNUC__) && \
     ((__GNUC__ >= 3) || \
      (__GNUC__ == 2) && (__GNUC_MINOR__ >= 5)))
#  define _HPy_NO_RETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#  define _HPy_NO_RETURN __declspec(noreturn)
#else
#  define _HPy_NO_RETURN
#endif


#ifdef HPY_UNIVERSAL_ABI
#    include "hpy/universal/hpy.h"
#else
#    include "hpy/cpython/hpy.h"
#endif

#endif /* HPy_H */
