#ifndef HPY_MISC_TRAMPOLINES_H
#define HPY_MISC_TRAMPOLINES_H

static inline HPy _HPy_New(HPyContext *ctx, HPy h_type, void **data) {
    /* Performance hack: the autogenerated version of this trampoline would
       simply forward data to ctx_New.

       Suppose you call HPy_New this way:
           PointObject *point;
           HPy h = HPy_New(ctx, cls, &point);

       If you pass "data" to ctx->New, the C compiler must assume that anybody
       could write a different value at any time into this local variable
       because a pointer to it escaped. With this hack, it's no longer the
       case: what escaped is the address of data_result instead and that local
       variable disappears since this function is likely inlined.

       See https://github.com/pyhandle/hpy/pull/22#pullrequestreview-413365845
    */
    void *data_result;
    HPy h = ctx->ctx_New(ctx, h_type, &data_result);
    *data = data_result;
    return h;
}

static inline _HPy_NO_RETURN void
HPy_FatalError(HPyContext *ctx, const char *message) {
    ctx->ctx_FatalError(ctx, message);
    /* note: the following abort() is unreachable, but needed because the
       _HPy_NO_RETURN doesn't seem to be sufficient.  I think that what
       occurs is that this function is inlined, after which gcc forgets
       that it couldn't return.  Having abort() inlined fixes that. */
    abort();
}

static inline void *
HPyCapsule_GetPointer(HPyContext *ctx, HPy capsule, const char *name)
{
    return ctx->ctx_Capsule_Get(
            ctx, capsule, HPyCapsule_key_Pointer, name);
}

static inline const char *
HPyCapsule_GetName(HPyContext *ctx, HPy capsule)
{
    return (const char *) ctx->ctx_Capsule_Get(
            ctx, capsule, HPyCapsule_key_Name, NULL);
}

static inline void *
HPyCapsule_GetContext(HPyContext *ctx, HPy capsule)
{
    return ctx->ctx_Capsule_Get(
            ctx, capsule, HPyCapsule_key_Context, NULL);
}

static inline int
HPyCapsule_SetPointer(HPyContext *ctx, HPy capsule, void *pointer)
{
    return ctx->ctx_Capsule_Set(
            ctx, capsule, HPyCapsule_key_Pointer, pointer);
}

static inline int
HPyCapsule_SetName(HPyContext *ctx, HPy capsule, const char *name)
{
    return ctx->ctx_Capsule_Set(
            ctx, capsule, HPyCapsule_key_Name, (void *) name);
}

static inline int
HPyCapsule_SetContext(HPyContext *ctx, HPy capsule, void *context)
{
    return ctx->ctx_Capsule_Set(
            ctx, capsule, HPyCapsule_key_Context, context);
}

static inline int
HPyCapsule_SetDestructor(HPyContext *ctx, HPy capsule,
        HPyCapsule_Destructor *destructor)
{
    return ctx->ctx_Capsule_Set(
            ctx, capsule, HPyCapsule_key_Destructor, (void *) destructor);
}

#endif /* HPY_MISC_TRAMPOLINES_H */
