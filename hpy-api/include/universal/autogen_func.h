static inline HPy
HPyModule_Create(HPyContext ctx, HPyModuleDef *def)
{
    // XXX: think about versioning
    return ctx->module_Create(ctx, def);
}

static inline HPy
HPyNone_Get(HPyContext ctx)
{
    return ctx->none_Get(ctx);
}

static inline HPy
HPy_FromPyObject(HPyContext ctx, struct _object *obj)
{
    return ctx->fromPyObject(ctx, obj);
}

static inline struct _object *
HPy_AsPyObject(HPyContext ctx, HPy h)
{
    return ctx->asPyObject(ctx, h);
}

static inline HPy
HPy_Dup(HPyContext ctx, HPy h)
{
    return ctx->dup(ctx, h);
}

static inline void
HPy_Close(HPyContext ctx, HPy h)
{
    ctx->close(ctx, h);
}

static inline HPy
HPyLong_FromLong(HPyContext ctx, long value)
{
    return ctx->long_FromLong(ctx, value);
}

static inline int
HPyArg_ParseTuple(HPyContext ctx, HPy args, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int res = ctx->arg_VaParse(ctx, args, fmt, vl);
    va_end(vl);
    return res;
}

static inline HPy
HPyNumber_Add(HPyContext ctx, HPy x, HPy y)
{
    return ctx->number_Add(ctx, x, y);
}
