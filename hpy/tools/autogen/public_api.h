/* these are not real typedefs: they are there only to make pycparser happy */
typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef int HPyType_Spec;
typedef int HPyCFunction;
typedef int HPy_ssize_t;
typedef int wchar_t;
typedef int HPyFunc_Signature;
typedef int cpy_PyObject;


/* HPy public API */

HPy h_None;
HPy h_True;
HPy h_False;
HPy h_ValueError;
HPy h_TypeError;
HPy h_BaseObjectType;   /* built-in 'object' */
HPy h_TypeType;         /* built-in 'type' */
HPy h_LongType;         /* built-in 'int' */
HPy h_UnicodeType;      /* built-in 'str' */
HPy h_TupleType;        /* built-in 'tuple' */
HPy h_ListType;         /* built-in 'list' */

HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def);
HPy HPy_Dup(HPyContext ctx, HPy h);
void HPy_Close(HPyContext ctx, HPy h);
HPy HPyLong_FromLong(HPyContext ctx, long value);
HPy HPyLong_FromLongLong(HPyContext ctx, long long v);
HPy HPyLong_FromUnsignedLongLong(HPyContext ctx, unsigned long long v);

long HPyLong_AsLong(HPyContext ctx, HPy h);
HPy HPyFloat_FromDouble(HPyContext ctx, double v);
double HPyFloat_AsDouble(HPyContext ctx, HPy h);

/* abstract.h */
HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Subtract(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Multiply(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_MatrixMultiply(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_FloorDivide(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_TrueDivide(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Remainder(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Divmod(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Power(HPyContext ctx, HPy h1, HPy h2, HPy h3);
HPy HPy_Negative(HPyContext ctx, HPy h1);
HPy HPy_Positive(HPyContext ctx, HPy h1);
HPy HPy_Absolute(HPyContext ctx, HPy h1);
HPy HPy_Invert(HPyContext ctx, HPy h1);
HPy HPy_Lshift(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Rshift(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_And(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Xor(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Or(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_Index(HPyContext ctx, HPy h1);
HPy HPy_Long(HPyContext ctx, HPy h1);
HPy HPy_Float(HPyContext ctx, HPy h1);

HPy HPy_InPlaceAdd(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceSubtract(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceMultiply(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceMatrixMultiply(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceFloorDivide(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceTrueDivide(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceRemainder(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlacePower(HPyContext ctx, HPy h1, HPy h2, HPy h3);
HPy HPy_InPlaceLshift(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceRshift(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceAnd(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceXor(HPyContext ctx, HPy h1, HPy h2);
HPy HPy_InPlaceOr(HPyContext ctx, HPy h1, HPy h2);

/* pyerrors.h */
void HPyErr_SetString(HPyContext ctx, HPy h_type, const char *message);
/* note: HPyErr_Occurred() returns a flag 0-or-1, instead of a 'PyObject *' */
int HPyErr_Occurred(HPyContext ctx);

/* object.h */
int HPy_IsTrue(HPyContext ctx, HPy h);
HPy HPyType_FromSpec(HPyContext ctx, HPyType_Spec *spec);

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
HPy _HPy_New(HPyContext ctx, HPy h_type, void **data);

HPy HPy_Repr(HPyContext ctx, HPy obj);
HPy HPy_Str(HPyContext ctx, HPy obj);
HPy HPy_ASCII(HPyContext ctx, HPy obj);
HPy HPy_Bytes(HPyContext ctx, HPy obj);

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
HPy HPy_FromPyObject(HPyContext ctx, cpy_PyObject *obj);
cpy_PyObject *HPy_AsPyObject(HPyContext ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
void _HPy_CallRealFunctionFromTrampoline(HPyContext ctx,
                                         HPyFunc_Signature sig,
                                         void *func,
                                         void *args);




/* *******
   hpyfunc
   *******

   These typedefs are used to generate the various macros used by
   include/common/hpyfunc.h
*/
typedef HPy (*HPyFunc_noargs)(HPyContext ctx, HPy self);
typedef HPy (*HPyFunc_o)(HPyContext ctx, HPy self, HPy arg);
typedef HPy (*HPyFunc_varargs)(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs);
typedef HPy (*HPyFunc_keywords)(HPyContext ctx, HPy self,
                                HPy *args, HPy_ssize_t nargs, HPy kw);

typedef HPy (*HPyFunc_unaryfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_binaryfunc)(HPyContext ctx, HPy, HPy);
typedef HPy (*HPyFunc_ternaryfunc)(HPyContext ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_inquiry)(HPyContext ctx, HPy);
typedef HPy_ssize_t (*HPyFunc_lenfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_ssizeargfunc)(HPyContext ctx, HPy, HPy_ssize_t);
typedef HPy (*HPyFunc_ssizessizeargfunc)(HPyContext ctx, HPy, HPy_ssize_t, HPy_ssize_t);
typedef int (*HPyFunc_ssizeobjargproc)(HPyContext ctx, HPy, HPy_ssize_t, HPy);
typedef int (*HPyFunc_ssizessizeobjargproc)(HPyContext ctx, HPy, HPy_ssize_t, HPy_ssize_t, HPy);
typedef int (*HPyFunc_objobjargproc)(HPyContext ctx, HPy, HPy, HPy);
typedef void (*HPyFunc_freefunc)(HPyContext ctx, void *);
typedef void (*HPyFunc_destructor)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_getattrfunc)(HPyContext ctx, HPy, char *);
typedef HPy (*HPyFunc_getattrofunc)(HPyContext ctx, HPy, HPy);
typedef int (*HPyFunc_setattrfunc)(HPyContext ctx, HPy, char *, HPy);
typedef int (*HPyFunc_setattrofunc)(HPyContext ctx, HPy, HPy, HPy);
typedef HPy (*HPyFunc_reprfunc)(HPyContext ctx, HPy);
//typedef Py_hash_t (*HPyFunc_hashfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_richcmpfunc)(HPyContext ctx, HPy, HPy, int);
typedef HPy (*HPyFunc_getiterfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_iternextfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_descrgetfunc)(HPyContext ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_descrsetfunc)(HPyContext ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_initproc)(HPyContext ctx, HPy, HPy, HPy);
