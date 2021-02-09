/* We define HPy_New as a macro around _HPy_New to suppress a
   warning. Usually, we expected it to be called this way:

       PointObject *p;
       HPy h = HPy_New(ctx, cls, &p);

   If we call _HPy_New directly, we get a warning because we are implicitly
   casting a PointObject** into a void**. The following macro explicitly
   casts the third argument to a void**.

   Note: Previously this macro used a trick to cast to void** in a safer
   way by doing the following:

       (void)sizeof((*data)->_ob_head._reserved0), (void**)data / * Error: blah blah * /

   The left operand of the comma operator is ignored, but it ensures that
   p is really a pointer to a pointer to a structure with a PyObject_HEAD.
   The sizeof() ensures that the expression is not actually evaluated at all.
   The cast to (void) is needed to convince GCC not to emit a warning about
   the unused result.

   The goal of the comment inside the macro is to be displayed in case of
   error. If you pass a non compatible type (e.g. a PointObject* instead of a
   PointObject**), GCC will complain with an error like this:

     ...macros.h:28:19: error: invalid type argument of unary ‘*’ (...)
     (void)sizeof((*data)->_ob_head._reserved0), / * ERROR: blah blah * /

   Unfortunately, HPy_New now supports non-legacy types which do not contain
   PyObject_HEAD, so this check is no longer correct in call cases.
*/

#define HPy_New(ctx, cls, data) (_HPy_New(                                    \
    (ctx),                                                                    \
    (cls),                                                                    \
    ((void**)data)                                                            \
  ))

/* A macro for creating (static inline) helper functions for custom types.

   Two versions of the helper exist. One for legacy types and one for pure
   HPy types.

   Example for a pure HPy custom type:

       HPy_TYPE_HELPERS(PointObject)

   This would generate a static inline function named HPy_AsPointObject that
   uses HPy_AsStruct to return the PointObject struct associated with a given
   handle.

   Example for a legacy custom type:

       HPy_LEGACY_TYPE_HELPERS(PointObject)

   This would generate a similar function that used HPy_AsStructLegacy instead.
*/

#define HPyType_HELPERS(TYPE) \
    _HPyType_GENERIC_HELPERS(TYPE, HPy_AsStruct, /* no struct header */, 0)

#define HPyType_LEGACY_HELPERS(TYPE) \
    _HPyType_GENERIC_HELPERS(TYPE, HPy_AsStructLegacy, PyObject_HEAD, 1)

#define _HPyType_GENERIC_HELPERS(TYPE, CAST, STRUCT_HEADER, STRUCT_IS_LEGACY) \
                                                                     \
static const int __attribute__((unused)) TYPE##_STRUCT_IS_LEGACY     \
    = STRUCT_IS_LEGACY;                                              \
                                                                     \
static inline __attribute__((unused)) TYPE *                         \
HPy_As##TYPE(HPyContext ctx, HPy h)                                  \
{                                                                    \
    return (TYPE*) CAST(ctx, h);                                     \
}


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
