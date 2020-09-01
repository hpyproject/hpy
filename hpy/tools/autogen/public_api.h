/* these are not real typedefs: they are there only to make pycparser happy */
typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef int HPyType_Spec;
typedef int HPyCFunction;
typedef int HPy_ssize_t;
typedef int HPy_hash_t;
typedef int wchar_t;
typedef int size_t;
typedef int HPyFunc_Signature;
typedef int cpy_PyObject;
typedef int HPyListBuilder;


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
HPy HPyLong_FromUnsignedLong(HPyContext ctx, unsigned long value);
HPy HPyLong_FromLongLong(HPyContext ctx, long long v);
HPy HPyLong_FromUnsignedLongLong(HPyContext ctx, unsigned long long v);
HPy HPyLong_FromSize_t(HPyContext ctx, size_t value);
HPy HPyLong_FromSsize_t(HPyContext ctx, HPy_ssize_t value);

long HPyLong_AsLong(HPyContext ctx, HPy h);
HPy HPyFloat_FromDouble(HPyContext ctx, double v);
double HPyFloat_AsDouble(HPyContext ctx, HPy h);

/* abstract.h */
HPy_ssize_t HPy_Length(HPyContext ctx, HPy h);

int HPyNumber_Check(HPyContext ctx, HPy h);
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
HPy HPyErr_NoMemory(HPyContext ctx);

/* object.h */
int HPy_IsTrue(HPyContext ctx, HPy h);
HPy HPyType_FromSpec(HPyContext ctx, HPyType_Spec *spec);
HPy HPyType_GenericNew(HPyContext ctx, HPy type, HPy *args, HPy_ssize_t nargs, HPy kw);

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

HPy HPy_RichCompare(HPyContext ctx, HPy v, HPy w, int op);
int HPy_RichCompareBool(HPyContext ctx, HPy v, HPy w, int op);

HPy_hash_t HPy_Hash(HPyContext ctx, HPy obj);

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
int HPyList_Check(HPyContext ctx, HPy h);
HPy HPyList_New(HPyContext ctx, HPy_ssize_t len);
int HPyList_Append(HPyContext ctx, HPy h_list, HPy h_item);

/* dictobject.h */
int HPyDict_Check(HPyContext ctx, HPy h);
HPy HPyDict_New(HPyContext ctx);
// TODO: Move HPyDict_SetItem and _GetItem (we have HPy_SetItem and _GetItem)
//       out of the base API.
// TODO: Leave name as HPyDict_SetItem (it doesn't steal).
// TODO: Rename to HPyDict_GetItem_NewHandle.
int HPyDict_SetItem(HPyContext ctx, HPy h_dict, HPy h_key, HPy h_val);
HPy HPyDict_GetItem(HPyContext ctx, HPy h_dict, HPy h_key);

/* tupleobject.h */
HPy HPyTuple_FromArray(HPyContext ctx, HPy items[], HPy_ssize_t n);
// note: HPyTuple_Pack is implemented as a macro in common/macros.h


/* integration with the old CPython API */
HPy HPy_FromPyObject(HPyContext ctx, cpy_PyObject *obj);
cpy_PyObject *HPy_AsPyObject(HPyContext ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
void _HPy_CallRealFunctionFromTrampoline(HPyContext ctx,
                                         HPyFunc_Signature sig,
                                         void *func,
                                         void *args);
void _HPy_CallDestroyAndThenDealloc(HPyContext ctx,
                                    void *func,
                                    cpy_PyObject *self);


/* Builders */

HPyListBuilder HPyListBuilder_New(HPyContext ctx, HPy_ssize_t initial_size);
void HPyListBuilder_Set(HPyContext ctx, HPyListBuilder builder,
                        HPy_ssize_t index, HPy h_item);
HPy HPyListBuilder_Build(HPyContext ctx, HPyListBuilder builder);
void HPyListBuilder_Cancel(HPyContext ctx, HPyListBuilder builder);



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
typedef HPy (*HPyFunc_getattrfunc)(HPyContext ctx, HPy, char *);
typedef HPy (*HPyFunc_getattrofunc)(HPyContext ctx, HPy, HPy);
typedef int (*HPyFunc_setattrfunc)(HPyContext ctx, HPy, char *, HPy);
typedef int (*HPyFunc_setattrofunc)(HPyContext ctx, HPy, HPy, HPy);
typedef HPy (*HPyFunc_reprfunc)(HPyContext ctx, HPy);
typedef HPy_hash_t (*HPyFunc_hashfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_richcmpfunc)(HPyContext ctx, HPy, HPy, int);
typedef HPy (*HPyFunc_getiterfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_iternextfunc)(HPyContext ctx, HPy);
typedef HPy (*HPyFunc_descrgetfunc)(HPyContext ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_descrsetfunc)(HPyContext ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_initproc)(HPyContext ctx, HPy self,
                                HPy *args, HPy_ssize_t nargs, HPy kw);
typedef HPy (*HPyFunc_getter)(HPyContext ctx, HPy, void *);
typedef int (*HPyFunc_setter)(HPyContext ctx, HPy, HPy, void *);

typedef void (*HPyFunc_destroyfunc)(void *);


/* ~~~ HPySlot_Slot ~~~

   The following enum is used to generate autogen_hpyslot.h, which contains:

     - The real definition of the enum HPySlot_Slot

     - the macros #define _HPySlot_SIGNATURE_*

*/

// NOTE: if you uncomment/enable a slot below, make sure to write a corresponding
// test in test_slots.py

/* Note that the magic numbers are the same as CPython */
typedef enum {
    //HPy_mp_ass_subscript = SLOT(3, HPyFunc_X),
    //HPy_mp_length = SLOT(4, HPyFunc_X),
    //HPy_mp_subscript = SLOT(5, HPyFunc_X),
    HPy_nb_absolute = SLOT(6, HPyFunc_UNARYFUNC),
    HPy_nb_add = SLOT(7, HPyFunc_BINARYFUNC),
    HPy_nb_and = SLOT(8, HPyFunc_BINARYFUNC),
    HPy_nb_bool = SLOT(9, HPyFunc_INQUIRY),
    HPy_nb_divmod = SLOT(10, HPyFunc_BINARYFUNC),
    HPy_nb_float = SLOT(11, HPyFunc_UNARYFUNC),
    HPy_nb_floor_divide = SLOT(12, HPyFunc_BINARYFUNC),
    HPy_nb_index = SLOT(13, HPyFunc_UNARYFUNC),
    HPy_nb_inplace_add = SLOT(14, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_and = SLOT(15, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_floor_divide = SLOT(16, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_lshift = SLOT(17, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_multiply = SLOT(18, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_or = SLOT(19, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_power = SLOT(20, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_remainder = SLOT(21, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_rshift = SLOT(22, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_subtract = SLOT(23, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_true_divide = SLOT(24, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_xor = SLOT(25, HPyFunc_BINARYFUNC),
    HPy_nb_int = SLOT(26, HPyFunc_UNARYFUNC),
    HPy_nb_invert = SLOT(27, HPyFunc_UNARYFUNC),
    HPy_nb_lshift = SLOT(28, HPyFunc_BINARYFUNC),
    HPy_nb_multiply = SLOT(29, HPyFunc_BINARYFUNC),
    HPy_nb_negative = SLOT(30, HPyFunc_UNARYFUNC),
    HPy_nb_or = SLOT(31, HPyFunc_BINARYFUNC),
    HPy_nb_positive = SLOT(32, HPyFunc_UNARYFUNC),
    //HPy_nb_power = SLOT(33, HPyFunc_X),
    HPy_nb_remainder = SLOT(34, HPyFunc_BINARYFUNC),
    HPy_nb_rshift = SLOT(35, HPyFunc_BINARYFUNC),
    HPy_nb_subtract = SLOT(36, HPyFunc_BINARYFUNC),
    HPy_nb_true_divide = SLOT(37, HPyFunc_BINARYFUNC),
    HPy_nb_xor = SLOT(38, HPyFunc_BINARYFUNC),
    //HPy_sq_ass_item = SLOT(39, HPyFunc_X),
    //HPy_sq_concat = SLOT(40, HPyFunc_X),
    //HPy_sq_contains = SLOT(41, HPyFunc_X),
    //HPy_sq_inplace_concat = SLOT(42, HPyFunc_X),
    //HPy_sq_inplace_repeat = SLOT(43, HPyFunc_X),
    HPy_sq_item = SLOT(44, HPyFunc_SSIZEARGFUNC),
    //HPy_sq_length = SLOT(45, HPyFunc_X),
    //HPy_sq_repeat = SLOT(46, HPyFunc_X),
    //HPy_tp_alloc = SLOT(47, HPyFunc_X),      NOT SUPPORTED
    //HPy_tp_base = SLOT(48, HPyFunc_X),
    //HPy_tp_bases = SLOT(49, HPyFunc_X),
    //HPy_tp_call = SLOT(50, HPyFunc_X),
    //HPy_tp_clear = SLOT(51, HPyFunc_X),
    //HPy_tp_dealloc = SLOT(52, HPyFunc_X),    NOT SUPPORTED
    //HPy_tp_del = SLOT(53, HPyFunc_X),
    //HPy_tp_descr_get = SLOT(54, HPyFunc_X),
    //HPy_tp_descr_set = SLOT(55, HPyFunc_X),
    //HPy_tp_doc = SLOT(56, HPyFunc_X),
    //HPy_tp_getattr = SLOT(57, HPyFunc_X),
    //HPy_tp_getattro = SLOT(58, HPyFunc_X),
    //HPy_tp_hash = SLOT(59, HPyFunc_X),
    HPy_tp_init = SLOT(60, HPyFunc_INITPROC),
    //HPy_tp_is_gc = SLOT(61, HPyFunc_X),
    //HPy_tp_iter = SLOT(62, HPyFunc_X),
    //HPy_tp_iternext = SLOT(63, HPyFunc_X),
    //HPy_tp_methods = SLOT(64, HPyFunc_X),    NOT SUPPORTED
    HPy_tp_new = SLOT(65, HPyFunc_KEYWORDS),
    HPy_tp_repr = SLOT(66, HPyFunc_REPRFUNC),
    //HPy_tp_richcompare = SLOT(67, HPyFunc_X),
    //HPy_tp_setattr = SLOT(68, HPyFunc_X),
    //HPy_tp_setattro = SLOT(69, HPyFunc_X),
    //HPy_tp_str = SLOT(70, HPyFunc_X),
    //HPy_tp_traverse = SLOT(71, HPyFunc_X),
    //HPy_tp_members = SLOT(72, HPyFunc_X),    NOT SUPPORTED
    //HPy_tp_getset = SLOT(73, HPyFunc_X),     NOT SUPPORTED
    //HPy_tp_free = SLOT(74, HPyFunc_X),       NOT SUPPORTED
    HPy_nb_matrix_multiply = SLOT(75, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_matrix_multiply = SLOT(76, HPyFunc_BINARYFUNC),
    //HPy_am_await = SLOT(77, HPyFunc_X),
    //HPy_am_aiter = SLOT(78, HPyFunc_X),
    //HPy_am_anext = SLOT(79, HPyFunc_X),
    //HPy_tp_finalize = SLOT(80, HPyFunc_X),

    /* extra HPy slots */
    HPy_tp_destroy = SLOT(1000, HPyFunc_DESTROYFUNC),

} HPySlot_Slot;
