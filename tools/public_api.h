/* these are not real typedefs: they are there only to make pycparser happy */
typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef struct _object {
    int x;
} PyObject;
typedef int HPyCFunction;


/* HPy public API */

HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def);
HPy HPyNone_Get(HPyContext ctx);
HPy HPy_Dup(HPyContext ctx, HPy h);
void HPy_Close(HPyContext ctx, HPy h);
HPy HPyLong_FromLong(HPyContext ctx, long value);

int HPyArg_ParseTuple(HPyContext ctx, HPy args, const char *fmt, ...);
HPy HPyNumber_Add(HPyContext ctx, HPy x, HPy y);
HPy HPyUnicode_FromString(HPyContext ctx, const char *utf8);


/* integration with the old CPython API */
HPy HPy_FromPyObject(HPyContext ctx, struct _object *obj);
PyObject *HPy_AsPyObject(HPyContext ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
struct _object _HPy_CallRealFunctionFromTrampoline(HPyContext ctx,
                                                   PyObject *self,
                                                   PyObject *args,
                                                   HPyCFunction func);
