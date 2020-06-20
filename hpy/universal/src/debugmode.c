#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <execinfo.h>
#include "debugmode.h"


#define BT_BUF_SIZE 25

void
_hpy_fatal(const char *msg, ...)
{
    void *buffer[BT_BUF_SIZE];
    int nptrs;

    fprintf(stderr,
        "------------------------------------------------------------\n"
        "Fatal error in HPy (most recent call first):\n");
    nptrs = backtrace(buffer, BT_BUF_SIZE);
    backtrace_symbols_fd(buffer, nptrs, /*fd=*/ 2);

    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fprintf(stderr,
        "\n"
        "------------------------------------------------------------\n");

    abort();
}
