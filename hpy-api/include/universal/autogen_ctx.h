struct _HPyContext_s {
    int version;
    HPy (*module_Create)(HPyContext ctx, HPyModuleDef *def);
    HPy (*none_Get)(HPyContext ctx);
    struct _object *(*callRealFunctionFromTrampoline)(HPyContext ctx,
              struct _object *self, struct _object *args, HPyCFunction func);
    HPy (*fromPyObject)(HPyContext ctx, struct _object *obj);
    struct _object *(*asPyObject)(HPyContext ctx, HPy h);
    HPy (*dup)(HPyContext, HPy h);
    void (*close)(HPyContext, HPy h);
    HPy (*long_FromLong)(HPyContext, long value);
    int (*arg_VaParse)(HPyContext, HPy args, const char *fmt, va_list vl);
    HPy (*number_Add)(HPyContext, HPy x, HPy y);
    HPy (*unicode_FromString)(HPyContext, const char *utf8);
};
