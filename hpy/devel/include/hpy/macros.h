/* We define HPy_New as a macro around _HPy_New to suppress a
   warning. Usually, we expected it to be called this way:

       PointObject *p;
       HPy h = HPy_New(ctx, cls, &p);

   If we call _HPy_New directly, we get a warning because we are implicitly
   casting a PointObject** into a void**. The following macro explicitly
   casts the third argument to a void**.
*/

#define HPy_New(ctx, cls, data) (_HPy_New(                                    \
    (ctx),                                                                    \
    (cls),                                                                    \
    ((void**)data)                                                            \
  ))


/* ~~~ HPyTuple_Pack ~~~

   this is just syntactic sugar around HPyTuple_FromArray, to help porting the
   exising code which uses PyTuple_Pack
*/

#define HPyTuple_Pack(ctx, n, ...) (HPyTuple_FromArray(ctx, (HPy[]){ __VA_ARGS__ }, n))

/* Rich comparison opcodes */
typedef enum {
    HPy_LT = 0,
    HPy_LE = 1,
    HPy_EQ = 2,
    HPy_NE = 3,
    HPy_GT = 4,
    HPy_GE = 5,
} HPy_RichCmpOp;

// this needs to be a macro because val1 and val2 can be of arbitrary types
#define HPy_RETURN_RICHCOMPARE(ctx, val1, val2, op)                     \
    do {                                                                \
        bool result;                                                    \
        switch (op) {                                                   \
        case HPy_EQ: result = ((val1) == (val2)); break;                \
        case HPy_NE: result = ((val1) != (val2)); break;                \
        case HPy_LT: result = ((val1) <  (val2)); break;                \
        case HPy_GT: result = ((val1) >  (val2)); break;                \
        case HPy_LE: result = ((val1) <= (val2)); break;                \
        case HPy_GE: result = ((val1) >= (val2)); break;                \
        default:                                                        \
            HPy_FatalError(ctx, "Invalid value for HPy_RichCmpOp");     \
        }                                                               \
        if (result)                                                     \
            return HPy_Dup(ctx, ctx->h_True);                           \
        return HPy_Dup(ctx, ctx->h_False);                              \
    } while (0)


#if !defined(SIZEOF_PID_T) || SIZEOF_PID_T == SIZEOF_INT
    #define _HPy_PARSE_PID "i"
    #define HPyLong_FromPid HPyLong_FromLong
    #define HPyLong_AsPid HPyLong_AsLong
#elif SIZEOF_PID_T == SIZEOF_LONG
    #define _HPy_PARSE_PID "l"
    #define HPyLong_FromPid HPyLong_FromLong
    #define HPyLong_AsPid HPyLong_AsLong
#elif defined(SIZEOF_LONG_LONG) && SIZEOF_PID_T == SIZEOF_LONG_LONG
    #define _HPy_PARSE_PID "L"
    #define HPyLong_FromPid HPyLong_FromLongLong
    #define HPyLong_AsPid HPyLong_AsLongLong
#else
#error "sizeof(pid_t) is neither sizeof(int), sizeof(long) or sizeof(long long)"
#endif /* SIZEOF_PID_T */

/* Convenience macro that closes handle `op` and sets `op` to HPy_NULL.
 *
 * Note: the issues solved by Py_CLEAR on CPython do not apply to HPy. Since
 * handles should be always short-lived and never shared between threads or
 * different calls to HPy module entry points, arbitrary code triggered
 * from __del__ (which can be triggered from HPy_Close) cannot access the same
 * stale handle value.
 *
 * What can be shared are HPy_Fields, but there the
 * ``HPyField_Store(ctx, obj_h, &obj->f, HPy_NULL)`` should internally do the
 * right thing.
 */
#define HPy_CLEAR(ctx, op)          \
    do {                            \
        if (!HPy_IsNull(op)) {      \
            HPy_Close(ctx, op);     \
            (op) = HPy_NULL;        \
        }                           \
    } while (0)
