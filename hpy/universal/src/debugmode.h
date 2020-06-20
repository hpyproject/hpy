#ifndef HPY_DEBUGMODE_H
#define HPY_DEBUGMODE_H

void _hpy_fatal(const char *msg, ...);

#define HPY_ASSERT(condition, errargs)    \
    do {                                  \
        if (!(condition))                 \
            _hpy_fatal errargs;           \
    } while (0)

#endif /* HPY_DEBUGMODE_H */
