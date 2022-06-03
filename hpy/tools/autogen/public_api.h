typedef int HPy;
typedef int HPyContext;
typedef int HPyModuleDef;
typedef int HPyType_Spec;
typedef int HPyType_SpecParam;
typedef int HPyCFunction;
typedef int HPy_ssize_t;
typedef int HPy_hash_t;
typedef int wchar_t;
typedef int size_t;
typedef int HPyFunc_Signature;
typedef int cpy_PyObject;
typedef int HPyField;
typedef int HPyGlobal;
typedef int HPyListBuilder;
typedef int HPyTupleBuilder;
typedef int HPyTracker;
typedef int HPy_RichCmpOp;
typedef int HPy_buffer;
typedef int HPyFunc_visitproc;
typedef int HPy_UCS4;
typedef int HPyThreadState;
// ^ these are not real typedefs: they are there only to make pycparser happy
// and this comment is not on the top of the file, so that it is not picked up
// by autodoc

/* HPy public API */

/* Constants */
HPy h_None;
HPy h_True;
HPy h_False;
HPy h_NotImplemented;
HPy h_Ellipsis;

/* Exceptions */
HPy h_BaseException;
HPy h_Exception;
HPy h_StopAsyncIteration;
HPy h_StopIteration;
HPy h_GeneratorExit;
HPy h_ArithmeticError;
HPy h_LookupError;
HPy h_AssertionError;
HPy h_AttributeError;
HPy h_BufferError;
HPy h_EOFError;
HPy h_FloatingPointError;
HPy h_OSError;
HPy h_ImportError;
HPy h_ModuleNotFoundError;
HPy h_IndexError;
HPy h_KeyError;
HPy h_KeyboardInterrupt;
HPy h_MemoryError;
HPy h_NameError;
HPy h_OverflowError;
HPy h_RuntimeError;
HPy h_RecursionError;
HPy h_NotImplementedError;
HPy h_SyntaxError;
HPy h_IndentationError;
HPy h_TabError;
HPy h_ReferenceError;
HPy h_SystemError;
HPy h_SystemExit;
HPy h_TypeError;
HPy h_UnboundLocalError;
HPy h_UnicodeError;
HPy h_UnicodeEncodeError;
HPy h_UnicodeDecodeError;
HPy h_UnicodeTranslateError;
HPy h_ValueError;
HPy h_ZeroDivisionError;
HPy h_BlockingIOError;
HPy h_BrokenPipeError;
HPy h_ChildProcessError;
HPy h_ConnectionError;
HPy h_ConnectionAbortedError;
HPy h_ConnectionRefusedError;
HPy h_ConnectionResetError;
HPy h_FileExistsError;
HPy h_FileNotFoundError;
HPy h_InterruptedError;
HPy h_IsADirectoryError;
HPy h_NotADirectoryError;
HPy h_PermissionError;
HPy h_ProcessLookupError;
HPy h_TimeoutError;
// EnvironmentError, IOError and WindowsError are intentionally omitted (they
// are all aliases of OSError since Python 3.3).

/* Warnings */
HPy h_Warning;
HPy h_UserWarning;
HPy h_DeprecationWarning;
HPy h_PendingDeprecationWarning;
HPy h_SyntaxWarning;
HPy h_RuntimeWarning;
HPy h_FutureWarning;
HPy h_ImportWarning;
HPy h_UnicodeWarning;
HPy h_BytesWarning;
HPy h_ResourceWarning;

/* Types */
HPy h_BaseObjectType;   /* built-in 'object' */
HPy h_TypeType;         /* built-in 'type' */
HPy h_BoolType;         /* built-in 'bool' */
HPy h_LongType;         /* built-in 'int' */
HPy h_FloatType;        /* built-in 'float' */
HPy h_UnicodeType;      /* built-in 'str' */
HPy h_TupleType;        /* built-in 'tuple' */
HPy h_ListType;         /* built-in 'list' */

HPy HPyModule_Create(HPyContext *ctx, HPyModuleDef *def);
void* HPyModule_GetState(HPyContext *ctx, HPy module);

HPy HPy_Dup(HPyContext *ctx, HPy h);
void HPy_Close(HPyContext *ctx, HPy h);

HPy HPyLong_FromLong(HPyContext *ctx, long value);
HPy HPyLong_FromUnsignedLong(HPyContext *ctx, unsigned long value);
HPy HPyLong_FromLongLong(HPyContext *ctx, long long v);
HPy HPyLong_FromUnsignedLongLong(HPyContext *ctx, unsigned long long v);
HPy HPyLong_FromSize_t(HPyContext *ctx, size_t value);
HPy HPyLong_FromSsize_t(HPyContext *ctx, HPy_ssize_t value);

long HPyLong_AsLong(HPyContext *ctx, HPy h);
unsigned long HPyLong_AsUnsignedLong(HPyContext *ctx, HPy h);
unsigned long HPyLong_AsUnsignedLongMask(HPyContext *ctx, HPy h);
long long HPyLong_AsLongLong(HPyContext *ctx, HPy h);
unsigned long long HPyLong_AsUnsignedLongLong(HPyContext *ctx, HPy h);
unsigned long long HPyLong_AsUnsignedLongLongMask(HPyContext *ctx, HPy h);
size_t HPyLong_AsSize_t(HPyContext *ctx, HPy h);
HPy_ssize_t HPyLong_AsSsize_t(HPyContext *ctx, HPy h);
void* HPyLong_AsVoidPtr(HPyContext *ctx, HPy h);
double HPyLong_AsDouble(HPyContext *ctx, HPy h);

HPy HPyFloat_FromDouble(HPyContext *ctx, double v);
double HPyFloat_AsDouble(HPyContext *ctx, HPy h);

HPy HPyBool_FromLong(HPyContext *ctx, long v);


/* abstract.h */
HPy_ssize_t HPy_Length(HPyContext *ctx, HPy h);

int HPyNumber_Check(HPyContext *ctx, HPy h);
HPy HPy_Add(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Subtract(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Multiply(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_MatrixMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_FloorDivide(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_TrueDivide(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Remainder(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Divmod(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Power(HPyContext *ctx, HPy h1, HPy h2, HPy h3);
HPy HPy_Negative(HPyContext *ctx, HPy h1);
HPy HPy_Positive(HPyContext *ctx, HPy h1);
HPy HPy_Absolute(HPyContext *ctx, HPy h1);
HPy HPy_Invert(HPyContext *ctx, HPy h1);
HPy HPy_Lshift(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Rshift(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_And(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Xor(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Or(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_Index(HPyContext *ctx, HPy h1);
HPy HPy_Long(HPyContext *ctx, HPy h1);
HPy HPy_Float(HPyContext *ctx, HPy h1);

HPy HPy_InPlaceAdd(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceSubtract(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceMatrixMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceFloorDivide(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceTrueDivide(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceRemainder(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlacePower(HPyContext *ctx, HPy h1, HPy h2, HPy h3);
HPy HPy_InPlaceLshift(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceRshift(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceAnd(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceXor(HPyContext *ctx, HPy h1, HPy h2);
HPy HPy_InPlaceOr(HPyContext *ctx, HPy h1, HPy h2);

int HPyCallable_Check(HPyContext *ctx, HPy h);
HPy HPy_CallTupleDict(HPyContext *ctx, HPy callable, HPy args, HPy kw);

/* pyerrors.h */
void HPy_FatalError(HPyContext *ctx, const char *message);
HPy HPyErr_SetString(HPyContext *ctx, HPy h_type, const char *message);
HPy HPyErr_SetObject(HPyContext *ctx, HPy h_type, HPy h_value);
/* note: the filename will be FS decoded */
HPy HPyErr_SetFromErrnoWithFilename(HPyContext *ctx, HPy h_type, const char *filename_fsencoded);
HPy HPyErr_SetFromErrnoWithFilenameObjects(HPyContext *ctx, HPy h_type, HPy filename1, HPy filename2);
/* note: HPyErr_Occurred() returns a flag 0-or-1, instead of a 'PyObject *' */
int HPyErr_Occurred(HPyContext *ctx);
int HPyErr_ExceptionMatches(HPyContext *ctx, HPy exc);
HPy HPyErr_NoMemory(HPyContext *ctx);
void HPyErr_Clear(HPyContext *ctx);
HPy HPyErr_NewException(HPyContext *ctx, const char *name, HPy base, HPy dict);
HPy HPyErr_NewExceptionWithDoc(HPyContext *ctx, const char *name, const char *doc, HPy base, HPy dict);
int HPyErr_WarnEx(HPyContext *ctx, HPy category, const char *message, HPy_ssize_t stack_level);
void HPyErr_WriteUnraisable(HPyContext *ctx, HPy obj);

/* object.h */
int HPy_IsTrue(HPyContext *ctx, HPy h);
HPy HPyType_FromSpec(HPyContext *ctx, HPyType_Spec *spec,
                     HPyType_SpecParam *params);
HPy HPyType_GenericNew(HPyContext *ctx, HPy type, HPy *args, HPy_ssize_t nargs, HPy kw);

HPy HPy_GetAttr(HPyContext *ctx, HPy obj, HPy name);
HPy HPy_GetAttr_s(HPyContext *ctx, HPy obj, const char *name);

int HPy_HasAttr(HPyContext *ctx, HPy obj, HPy name);
int HPy_HasAttr_s(HPyContext *ctx, HPy obj, const char *name);

int HPy_SetAttr(HPyContext *ctx, HPy obj, HPy name, HPy value);
int HPy_SetAttr_s(HPyContext *ctx, HPy obj, const char *name, HPy value);

HPy HPy_GetItem(HPyContext *ctx, HPy obj, HPy key);
HPy HPy_GetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx);
HPy HPy_GetItem_s(HPyContext *ctx, HPy obj, const char *key);

int HPy_Contains(HPyContext *ctx, HPy container, HPy key);

int HPy_SetItem(HPyContext *ctx, HPy obj, HPy key, HPy value);
int HPy_SetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx, HPy value);
int HPy_SetItem_s(HPyContext *ctx, HPy obj, const char *key, HPy value);

HPy HPy_Type(HPyContext *ctx, HPy obj);
// WARNING: HPy_TypeCheck could be tweaked/removed in the future, see issue #160
int HPy_TypeCheck(HPyContext *ctx, HPy obj, HPy type);

int HPy_Is(HPyContext *ctx, HPy obj, HPy other);

void* HPy_AsStruct(HPyContext *ctx, HPy h);
void* HPy_AsStructLegacy(HPyContext *ctx, HPy h);

HPy _HPy_New(HPyContext *ctx, HPy h_type, void **data);

HPy HPy_Repr(HPyContext *ctx, HPy obj);
HPy HPy_Str(HPyContext *ctx, HPy obj);
HPy HPy_ASCII(HPyContext *ctx, HPy obj);
HPy HPy_Bytes(HPyContext *ctx, HPy obj);

HPy HPy_RichCompare(HPyContext *ctx, HPy v, HPy w, int op);
int HPy_RichCompareBool(HPyContext *ctx, HPy v, HPy w, int op);

HPy_hash_t HPy_Hash(HPyContext *ctx, HPy obj);

/* bytesobject.h */
int HPyBytes_Check(HPyContext *ctx, HPy h);
HPy_ssize_t HPyBytes_Size(HPyContext *ctx, HPy h);
HPy_ssize_t HPyBytes_GET_SIZE(HPyContext *ctx, HPy h);
char* HPyBytes_AsString(HPyContext *ctx, HPy h);
char* HPyBytes_AS_STRING(HPyContext *ctx, HPy h);
HPy HPyBytes_FromString(HPyContext *ctx, const char *v);
HPy HPyBytes_FromStringAndSize(HPyContext *ctx, const char *v, HPy_ssize_t len);

/* unicodeobject.h */
HPy HPyUnicode_FromString(HPyContext *ctx, const char *utf8);
int HPyUnicode_Check(HPyContext *ctx, HPy h);
HPy HPyUnicode_AsASCIIString(HPyContext *ctx, HPy h);
HPy HPyUnicode_AsLatin1String(HPyContext *ctx, HPy h);
HPy HPyUnicode_AsUTF8String(HPyContext *ctx, HPy h);
const char* HPyUnicode_AsUTF8AndSize(HPyContext *ctx, HPy h, HPy_ssize_t *size);
HPy HPyUnicode_FromWideChar(HPyContext *ctx, const wchar_t *w, HPy_ssize_t size);
HPy HPyUnicode_DecodeFSDefault(HPyContext *ctx, const char* v);
HPy HPyUnicode_DecodeFSDefaultAndSize(HPyContext *ctx, const char* v, HPy_ssize_t size);
HPy HPyUnicode_EncodeFSDefault(HPyContext *ctx, HPy h);
HPy_UCS4 HPyUnicode_ReadChar(HPyContext *ctx, HPy h, HPy_ssize_t index);
HPy HPyUnicode_DecodeASCII(HPyContext *ctx, const char *s, HPy_ssize_t size, const char *errors);
HPy HPyUnicode_DecodeLatin1(HPyContext *ctx, const char *s, HPy_ssize_t size, const char *errors);

/* listobject.h */
int HPyList_Check(HPyContext *ctx, HPy h);
HPy HPyList_New(HPyContext *ctx, HPy_ssize_t len);
int HPyList_Append(HPyContext *ctx, HPy h_list, HPy h_item);

/* dictobject.h */
int HPyDict_Check(HPyContext *ctx, HPy h);
HPy HPyDict_New(HPyContext *ctx);

/* tupleobject.h */
int HPyTuple_Check(HPyContext *ctx, HPy h);
HPy HPyTuple_FromArray(HPyContext *ctx, HPy items[], HPy_ssize_t n);
// note: HPyTuple_Pack is implemented as a macro in common/macros.h

/* import.h */
HPy HPyImport_ImportModule(HPyContext *ctx, const char *name);

/* integration with the old CPython API */
HPy HPy_FromPyObject(HPyContext *ctx, cpy_PyObject *obj);
cpy_PyObject *HPy_AsPyObject(HPyContext *ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
void _HPy_CallRealFunctionFromTrampoline(HPyContext *ctx,
                                         HPyFunc_Signature sig,
                                         HPyCFunction func,
                                         void *args);

/* Builders */

HPyListBuilder HPyListBuilder_New(HPyContext *ctx, HPy_ssize_t initial_size);
void HPyListBuilder_Set(HPyContext *ctx, HPyListBuilder builder,
                        HPy_ssize_t index, HPy h_item);
HPy HPyListBuilder_Build(HPyContext *ctx, HPyListBuilder builder);
void HPyListBuilder_Cancel(HPyContext *ctx, HPyListBuilder builder);

HPyTupleBuilder HPyTupleBuilder_New(HPyContext *ctx, HPy_ssize_t initial_size);
void HPyTupleBuilder_Set(HPyContext *ctx, HPyTupleBuilder builder,
                         HPy_ssize_t index, HPy h_item);
HPy HPyTupleBuilder_Build(HPyContext *ctx, HPyTupleBuilder builder);
void HPyTupleBuilder_Cancel(HPyContext *ctx, HPyTupleBuilder builder);

/* Helper for correctly closing handles */

HPyTracker HPyTracker_New(HPyContext *ctx, HPy_ssize_t size);
int HPyTracker_Add(HPyContext *ctx, HPyTracker ht, HPy h);
void HPyTracker_ForgetAll(HPyContext *ctx, HPyTracker ht);
void HPyTracker_Close(HPyContext *ctx, HPyTracker ht);

/**
 * HPyFields should be used ONLY in parts of memory which is known to the GC,
 * e.g. memory allocated by HPy_New:
 *
 *   - NEVER declare a local variable of type HPyField
 *   - NEVER use HPyField on a struct allocated by e.g. malloc()
 *
 * **CPython's note**: contrarily than PyObject*, you don't need to manually
 * manage refcounting when using HPyField: if you use HPyField_Store to
 * overwrite an existing value, the old object will be automatically decrefed.
 * This means that you CANNOT use HPyField_Store to write memory which
 * contains uninitialized values, because it would try to decref a dangling
 * pointer.
 *
 * Note that HPy_New automatically zeroes the memory it allocates, so
 * everything works well out of the box. In case you are using manually
 * allocated memory, you should initialize the HPyField to HPyField_NULL.
 *
 * Note the difference:
 *
 *   - ``obj->f = HPyField_NULL``: this should be used only to initialize
 *     uninitialized memory. If you use it to overwrite a valid HPyField, you
 *     will cause a memory leak (at least on CPython)
 *
 *   - HPyField_Store(ctx, &obj->f, HPy_NULL): this does the right and decref
 *     the old value. However, you CANNOT use it if the memory is not
 *     initialized.
 *
 * Note: target_object and source_object are there in case an implementation
 * needs to add write and/or read barriers on the objects. They are ignored by
 * CPython but e.g. PyPy needs a write barrier.
*/
void HPyField_Store(HPyContext *ctx, HPy target_object, HPyField *target_field, HPy h);
HPy HPyField_Load(HPyContext *ctx, HPy source_object, HPyField source_field);

/**
 * Leaving Python execution: for releasing GIL and other use-cases.
 *
 * In most situations, users should prefer using convenience macros:
 * HPy_BEGIN_LEAVE_PYTHON(context)/HPy_END_LEAVE_PYTHON(context)
 *
 * HPy extensions may leave Python execution when running Python independent
 * code: long-running computations or blocking operations. When an extension
 * has left the Python execution it must not call any HPy API other than
 * HPy_ReenterPythonExecution. It can access pointers returned by HPy API,
 * e.g., HPyUnicode_AsUTF8String, provided that they are valid at the point
 * of calling HPy_LeavePythonExecution.
 *
 * Python execution must be reentered on the same thread as where it was left.
 * The leave/enter calls must not be nested. Debug mode will, in the future,
 * enforce these constraints.
 *
 * Python implementations may use this knowledge however they wish. The most
 * obvious use case is to release the GIL, in which case the
 * HPy_BEGIN_LEAVE_PYTHON/HPy_END_LEAVE_PYTHON becomes equivalent to
 * Py_BEGIN_ALLOW_THREADS/Py_END_ALLOW_THREADS.
*/
void HPy_ReenterPythonExecution(HPyContext *ctx, HPyThreadState state);
HPyThreadState HPy_LeavePythonExecution(HPyContext *ctx);

/**
 * HPyGlobal is an alternative to module state. HPyGlobal must be a statically
 * allocated C global variable registered in HPyModuleDef.globals array.
 * A HPyGlobal can be used only after the HPy module where it is registered was
 * created using HPyModule_Create.
 *
 * HPyGlobal serves as an identifier of a Python object that should be globally
 * available per one Python interpreter. Python objects referenced by HPyGlobals
 * are destroyed automatically on the interpreter exit (not necessarily the
 * process exit).
 *
 * HPyGlobal instance does not allow anything else but loading and storing
 * a HPy handle using a HPyContext. Even if the HPyGlobal C variable may
 * be shared between threads or different interpreter instances within one
 * process, the API to load and store a handle from HPyGlobal is thread-safe (but
 * like any other HPy API must not be called in HPy_LeavePythonExecution blocks).
 *
 * Given that a handle to object X1 is stored to HPyGlobal using HPyContext of
 * Python interpreter I1, then loading a handle from the same HPyGlobal using
 * HPyContext of Python interpreter I1 should give a handle to the same object
 * X1. Another Python interpreter I2 running within the same process and using
 * the same HPyGlobal variable will not be able to load X1 from it, it will have
 * its own view on what is stored in the given HPyGlobal.
 *
 * Python interpreters may use indirection to isolate different interpreter
 * instances, but alternative techniques such as copy-on-write or immortal
 * objects can be used to avoid that indirection (even selectively on per
 * object basis using tagged pointers).
 *
 * CPython HPy implementation may even provide configuration option that
 * switches between a faster version that stores directly PyObject* to
 * HPyGlobal but does not support subinterpreters, or a version that supports
 * subinterpreters. For now, CPython HPy always stores PyObject* directly
 * to HPyGlobal.
 *
 * While the standard implementation does not fully enforce the documented
 * contract, the HPy debug mode will enforce it (not implemented yet).
 *
 * **Implementation notes:**
 * All Python interpreters running in one process must be compatible, because
 * they will share all HPyGlobal C level variables. The internal data stored
 * in HPyGlobal are specific for each HPy implementation, each implementation
 * is also responsible for handling thread-safety when initializing the
 * internal data in HPyModule_Create. Note that HPyModule_Create may be called
 * concurrently depending on the semantics of the Python implementation (GIL vs
 * no GIL) and also depending on the whether there may be multiple instances of
 * given Python interpreter running within the same process. In the future, HPy
 * ABI may include a contract that internal data of each HPyGlobal must be
 * initialized to its address using atomic write and HPy implementations will
 * not be free to choose what to store in HPyGlobal, however, this will allow
 * multiple different HPy implementations within one process. This contract may
 * also be activated only by some runtime option, letting the HPy implementation
 * use more optimized HPyGlobal implementation otherwise.
*/
void HPyGlobal_Store(HPyContext *ctx, HPyGlobal *global, HPy h);
HPy HPyGlobal_Load(HPyContext *ctx, HPyGlobal global);

/* Debugging helpers */
void _HPy_Dump(HPyContext *ctx, HPy h);


/* *******
   hpyfunc
   *******

   These typedefs are used to generate the various macros used by
   include/common/hpyfunc.h
*/
typedef HPy (*HPyFunc_noargs)(HPyContext *ctx, HPy self);
typedef HPy (*HPyFunc_o)(HPyContext *ctx, HPy self, HPy arg);
typedef HPy (*HPyFunc_varargs)(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs);
typedef HPy (*HPyFunc_keywords)(HPyContext *ctx, HPy self,
                                HPy *args, HPy_ssize_t nargs, HPy kw);

typedef HPy (*HPyFunc_unaryfunc)(HPyContext *ctx, HPy);
typedef HPy (*HPyFunc_binaryfunc)(HPyContext *ctx, HPy, HPy);
typedef HPy (*HPyFunc_ternaryfunc)(HPyContext *ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_inquiry)(HPyContext *ctx, HPy);
typedef HPy_ssize_t (*HPyFunc_lenfunc)(HPyContext *ctx, HPy);
typedef HPy (*HPyFunc_ssizeargfunc)(HPyContext *ctx, HPy, HPy_ssize_t);
typedef HPy (*HPyFunc_ssizessizeargfunc)(HPyContext *ctx, HPy, HPy_ssize_t, HPy_ssize_t);
typedef int (*HPyFunc_ssizeobjargproc)(HPyContext *ctx, HPy, HPy_ssize_t, HPy);
typedef int (*HPyFunc_ssizessizeobjargproc)(HPyContext *ctx, HPy, HPy_ssize_t, HPy_ssize_t, HPy);
typedef int (*HPyFunc_objobjargproc)(HPyContext *ctx, HPy, HPy, HPy);
typedef void (*HPyFunc_freefunc)(HPyContext *ctx, void *);
typedef HPy (*HPyFunc_getattrfunc)(HPyContext *ctx, HPy, char *);
typedef HPy (*HPyFunc_getattrofunc)(HPyContext *ctx, HPy, HPy);
typedef int (*HPyFunc_setattrfunc)(HPyContext *ctx, HPy, char *, HPy);
typedef int (*HPyFunc_setattrofunc)(HPyContext *ctx, HPy, HPy, HPy);
typedef HPy (*HPyFunc_reprfunc)(HPyContext *ctx, HPy);
typedef HPy_hash_t (*HPyFunc_hashfunc)(HPyContext *ctx, HPy);
typedef HPy (*HPyFunc_richcmpfunc)(HPyContext *ctx, HPy, HPy, HPy_RichCmpOp);
typedef HPy (*HPyFunc_getiterfunc)(HPyContext *ctx, HPy);
typedef HPy (*HPyFunc_iternextfunc)(HPyContext *ctx, HPy);
typedef HPy (*HPyFunc_descrgetfunc)(HPyContext *ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_descrsetfunc)(HPyContext *ctx, HPy, HPy, HPy);
typedef int (*HPyFunc_initproc)(HPyContext *ctx, HPy self,
                                HPy *args, HPy_ssize_t nargs, HPy kw);
typedef HPy (*HPyFunc_getter)(HPyContext *ctx, HPy, void *);
typedef int (*HPyFunc_setter)(HPyContext *ctx, HPy, HPy, void *);
typedef int (*HPyFunc_objobjproc)(HPyContext *ctx, HPy, HPy);
typedef int (*HPyFunc_getbufferproc)(HPyContext *ctx, HPy, HPy_buffer *, int);
typedef void (*HPyFunc_releasebufferproc)(HPyContext *ctx, HPy, HPy_buffer *);
typedef int (*HPyFunc_traverseproc)(void *object, HPyFunc_visitproc visit, void *arg);
typedef void (*HPyFunc_destructor)(HPyContext *ctx, HPy);

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
    HPy_bf_getbuffer = SLOT(1, HPyFunc_GETBUFFERPROC),
    HPy_bf_releasebuffer = SLOT(2, HPyFunc_RELEASEBUFFERPROC),
    HPy_mp_ass_subscript = SLOT(3, HPyFunc_OBJOBJARGPROC),
    HPy_mp_length = SLOT(4, HPyFunc_LENFUNC),
    HPy_mp_subscript = SLOT(5, HPyFunc_BINARYFUNC),
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
    HPy_nb_inplace_power = SLOT(20, HPyFunc_TERNARYFUNC),
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
    HPy_nb_power = SLOT(33, HPyFunc_TERNARYFUNC),
    HPy_nb_remainder = SLOT(34, HPyFunc_BINARYFUNC),
    HPy_nb_rshift = SLOT(35, HPyFunc_BINARYFUNC),
    HPy_nb_subtract = SLOT(36, HPyFunc_BINARYFUNC),
    HPy_nb_true_divide = SLOT(37, HPyFunc_BINARYFUNC),
    HPy_nb_xor = SLOT(38, HPyFunc_BINARYFUNC),
    HPy_sq_ass_item = SLOT(39, HPyFunc_SSIZEOBJARGPROC),
    HPy_sq_concat = SLOT(40, HPyFunc_BINARYFUNC),
    HPy_sq_contains = SLOT(41, HPyFunc_OBJOBJPROC),
    HPy_sq_inplace_concat = SLOT(42, HPyFunc_BINARYFUNC),
    HPy_sq_inplace_repeat = SLOT(43, HPyFunc_SSIZEARGFUNC),
    HPy_sq_item = SLOT(44, HPyFunc_SSIZEARGFUNC),
    HPy_sq_length = SLOT(45, HPyFunc_LENFUNC),
    HPy_sq_repeat = SLOT(46, HPyFunc_SSIZEARGFUNC),
    //HPy_tp_alloc = SLOT(47, HPyFunc_X),      NOT SUPPORTED
    //HPy_tp_base = SLOT(48, HPyFunc_X),
    //HPy_tp_bases = SLOT(49, HPyFunc_X),
    //HPy_tp_call = SLOT(50, HPyFunc_X),
    //HPy_tp_clear = SLOT(51, HPyFunc_X),      NOT SUPPORTED, use tp_traverse
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
    HPy_tp_richcompare = SLOT(67, HPyFunc_RICHCMPFUNC),
    //HPy_tp_setattr = SLOT(68, HPyFunc_X),
    //HPy_tp_setattro = SLOT(69, HPyFunc_X),
    //HPy_tp_str = SLOT(70, HPyFunc_X),
    HPy_tp_traverse = SLOT(71, HPyFunc_TRAVERSEPROC),
    //HPy_tp_members = SLOT(72, HPyFunc_X),    NOT SUPPORTED
    //HPy_tp_getset = SLOT(73, HPyFunc_X),     NOT SUPPORTED
    //HPy_tp_free = SLOT(74, HPyFunc_X),       NOT SUPPORTED
    HPy_nb_matrix_multiply = SLOT(75, HPyFunc_BINARYFUNC),
    HPy_nb_inplace_matrix_multiply = SLOT(76, HPyFunc_BINARYFUNC),
    //HPy_am_await = SLOT(77, HPyFunc_X),
    //HPy_am_aiter = SLOT(78, HPyFunc_X),
    //HPy_am_anext = SLOT(79, HPyFunc_X),
    HPy_tp_finalize = SLOT(80, HPyFunc_DESTRUCTOR),

    /* extra HPy slots */
    HPy_tp_destroy = SLOT(1000, HPyFunc_DESTROYFUNC),
    HPy_m_traverse = SLOT(1001, HPyFunc_MODTRAVERSEPROC),
    HPy_m_destroy = SLOT(1002, HPyFunc_MODDESTROYFUNC),

} HPySlot_Slot;
