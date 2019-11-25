/* these are not real typedefs: they are there only to make pycparser happy */
typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef int HPyCFunction;
typedef int HPy_ssize_t;


/* HPy public API */

HPy h_None;
HPy h_True;
HPy h_False;
HPy h_ValueError;

HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def);
HPy HPy_Dup(HPyContext ctx, HPy h);
void HPy_Close(HPyContext ctx, HPy h);
HPy HPyLong_FromLong(HPyContext ctx, long value);
long HPyLong_AsLong(HPyContext ctx, HPy h);

int HPyArg_Parse(HPyContext ctx, HPy *args, HPy_ssize_t nargs,
                 const char *fmt, ...);
HPy HPyNumber_Add(HPyContext ctx, HPy x, HPy y);
HPy HPyUnicode_FromString(HPyContext ctx, const char *utf8);

void HPyErr_SetString(HPyContext ctx, HPy type, const char *message);

/* bytesobject.h */
int HPyBytes_Check(HPyContext ctx, HPy o);
HPy_ssize_t HPyBytes_Size(HPyContext ctx, HPy o);
HPy_ssize_t HPyBytes_GET_SIZE(HPyContext ctx, HPy o);
char* HPyBytes_AsString(HPyContext ctx, HPy o);
char* HPyBytes_AS_STRING(HPyContext ctx, HPy o);


/* integration with the old CPython API */
HPy HPy_FromPyObject(HPyContext ctx, struct _object *obj);
struct _object *HPy_AsPyObject(HPyContext ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
struct _object *_HPy_CallRealFunctionFromTrampoline(HPyContext ctx,
                                                    struct _object *self,
                                                    struct _object *args,
                                                    void *func,
                                                    int ml_flags);
