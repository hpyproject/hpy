/* these are not real typedefs: they are there only to make pycparser happy */
typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef int HPyType_Spec;
typedef int HPyCFunction;
typedef int HPy_ssize_t;
typedef int wchar_t;


/* HPy public API */

HPy h_None;
HPy h_True;
HPy h_False;
HPy h_ValueError;
HPy h_TypeError;

HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def);
HPy HPy_Dup(HPyContext ctx, HPy h);
void HPy_Close(HPyContext ctx, HPy h);
HPy HPyLong_FromLong(HPyContext ctx, long value);
HPy HPyLong_FromLongLong(HPyContext ctx, long long v);
HPy HPyLong_FromUnsignedLongLong(HPyContext ctx, unsigned long long v);

long HPyLong_AsLong(HPyContext ctx, HPy h);
HPy HPyFloat_FromDouble(HPyContext ctx, double v);

HPy HPyNumber_Add(HPyContext ctx, HPy h1, HPy h2);

/* pyerrors.h */
void HPyErr_SetString(HPyContext ctx, HPy h_type, const char *message);
/* note: HPyErr_Occurred() returns a flag 0-or-1, instead of a 'PyObject *' */
int HPyErr_Occurred(HPyContext ctx);

/* object.h */
int HPyObject_IsTrue(HPyContext ctx, HPy h);
HPy HPyType_FromSpec(HPyContext ctx, HPyType_Spec* spec);

HPy HPy_GetAttr(HPyContext ctx, HPy obj, HPy name);
HPy HPy_GetAttr_s(HPyContext ctx, HPy obj, const char *name);

int HPy_HasAttr(HPyContext ctx, HPy obj, HPy name);
int HPy_HasAttr_s(HPyContext ctx, HPy obj, const char *name);

int HPy_SetAttr(HPyContext ctx, HPy obj, HPy name, HPy value);
int HPy_SetAttr_s(HPyContext ctx, HPy obj, const char *name, HPy value);

HPy HPy_GetItem(HPyContext ctx, HPy obj, HPy key);
HPy HPy_GetItem_i(HPyContext ctx, HPy obj, HPy_ssize_t idx);
HPy HPy_GetItem_s(HPyContext ctx, HPy obj, const char *key);

int HPy_SetItem(HPyContext ctx, HPy obj, HPy key, HPy value);
int HPy_SetItem_i(HPyContext ctx, HPy obj, HPy_ssize_t idx, HPy value);
int HPy_SetItem_s(HPyContext ctx, HPy obj, const char *key, HPy value);

void* _HPy_Cast(HPyContext ctx, HPy h);
HPy HPy_New(HPyContext ctx, HPy h_type, void** data);

/* bytesobject.h */
int HPyBytes_Check(HPyContext ctx, HPy h);
HPy_ssize_t HPyBytes_Size(HPyContext ctx, HPy h);
HPy_ssize_t HPyBytes_GET_SIZE(HPyContext ctx, HPy h);
char* HPyBytes_AsString(HPyContext ctx, HPy h);
char* HPyBytes_AS_STRING(HPyContext ctx, HPy h);

/* unicodeobject.h */
HPy HPyUnicode_FromString(HPyContext ctx, const char *utf8);
int HPyUnicode_Check(HPyContext ctx, HPy h);
HPy HPyUnicode_AsUTF8String(HPyContext ctx, HPy h);
HPy HPyUnicode_FromWideChar(HPyContext ctx, const wchar_t *w, HPy_ssize_t size);

/* listobject.h */
HPy HPyList_New(HPyContext ctx, HPy_ssize_t len);
int HPyList_Append(HPyContext ctx, HPy h_list, HPy h_item);

/* dictobject.h */
HPy HPyDict_New(HPyContext ctx);
// TODO: Move HPyDict_SetItem and _GetItem (we have HPy_SetItem and _GetItem)
//       out of the base API.
// TODO: Leave name as HPyDict_SetItem (it doesn't steal).
// TODO: Rename to HPyDict_GetItem_NewHandle.
int HPyDict_SetItem(HPyContext ctx, HPy h_dict, HPy h_key, HPy h_val);
HPy HPyDict_GetItem(HPyContext ctx, HPy h_dict, HPy h_key);

/* integration with the old CPython API */
HPy HPy_FromPyObject(HPyContext ctx, struct _object *obj);
struct _object *HPy_AsPyObject(HPyContext ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
struct _object *_HPy_CallRealFunctionFromTrampoline(HPyContext ctx,
                                                    struct _object *self,
                                                    struct _object *args,
                                                    struct _object *kw,
                                                    void *func,
                                                    int ml_flags);
