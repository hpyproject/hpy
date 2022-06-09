/* HPy public API */

/*
 * IMPORTANT: In order to ensure backwards compatibility of HPyContext, it is
 * necessary to define the order of the context members. To do so, use macros
 * 'HPyAPI_HANDLE(idx)' for context handles and 'HPyAPI_FUNC(idx)' for
 * context functions. When adding members, it doesn't matter where they are
 * located in this file. It's just important that the maximum context index is
 * incremented by exactly one.
 */

#if AUTOGEN

/* Constants */
HPyAPI_HANDLE(0) HPy h_None;
HPyAPI_HANDLE(1) HPy h_True;
HPyAPI_HANDLE(2) HPy h_False;
HPyAPI_HANDLE(3) HPy h_NotImplemented;
HPyAPI_HANDLE(4) HPy h_Ellipsis;

/* Exceptions */
HPyAPI_HANDLE(5) HPy h_BaseException;
HPyAPI_HANDLE(6) HPy h_Exception;
HPyAPI_HANDLE(7) HPy h_StopAsyncIteration;
HPyAPI_HANDLE(8) HPy h_StopIteration;
HPyAPI_HANDLE(9) HPy h_GeneratorExit;
HPyAPI_HANDLE(10) HPy h_ArithmeticError;
HPyAPI_HANDLE(11) HPy h_LookupError;
HPyAPI_HANDLE(12) HPy h_AssertionError;
HPyAPI_HANDLE(13) HPy h_AttributeError;
HPyAPI_HANDLE(14) HPy h_BufferError;
HPyAPI_HANDLE(15) HPy h_EOFError;
HPyAPI_HANDLE(16) HPy h_FloatingPointError;
HPyAPI_HANDLE(17) HPy h_OSError;
HPyAPI_HANDLE(18) HPy h_ImportError;
HPyAPI_HANDLE(19) HPy h_ModuleNotFoundError;
HPyAPI_HANDLE(20) HPy h_IndexError;
HPyAPI_HANDLE(21) HPy h_KeyError;
HPyAPI_HANDLE(22) HPy h_KeyboardInterrupt;
HPyAPI_HANDLE(23) HPy h_MemoryError;
HPyAPI_HANDLE(24) HPy h_NameError;
HPyAPI_HANDLE(25) HPy h_OverflowError;
HPyAPI_HANDLE(26) HPy h_RuntimeError;
HPyAPI_HANDLE(27) HPy h_RecursionError;
HPyAPI_HANDLE(28) HPy h_NotImplementedError;
HPyAPI_HANDLE(29) HPy h_SyntaxError;
HPyAPI_HANDLE(30) HPy h_IndentationError;
HPyAPI_HANDLE(31) HPy h_TabError;
HPyAPI_HANDLE(32) HPy h_ReferenceError;
HPyAPI_HANDLE(33) HPy h_SystemError;
HPyAPI_HANDLE(34) HPy h_SystemExit;
HPyAPI_HANDLE(35) HPy h_TypeError;
HPyAPI_HANDLE(36) HPy h_UnboundLocalError;
HPyAPI_HANDLE(37) HPy h_UnicodeError;
HPyAPI_HANDLE(38) HPy h_UnicodeEncodeError;
HPyAPI_HANDLE(39) HPy h_UnicodeDecodeError;
HPyAPI_HANDLE(40) HPy h_UnicodeTranslateError;
HPyAPI_HANDLE(41) HPy h_ValueError;
HPyAPI_HANDLE(42) HPy h_ZeroDivisionError;
HPyAPI_HANDLE(43) HPy h_BlockingIOError;
HPyAPI_HANDLE(44) HPy h_BrokenPipeError;
HPyAPI_HANDLE(45) HPy h_ChildProcessError;
HPyAPI_HANDLE(46) HPy h_ConnectionError;
HPyAPI_HANDLE(47) HPy h_ConnectionAbortedError;
HPyAPI_HANDLE(48) HPy h_ConnectionRefusedError;
HPyAPI_HANDLE(49) HPy h_ConnectionResetError;
HPyAPI_HANDLE(50) HPy h_FileExistsError;
HPyAPI_HANDLE(51) HPy h_FileNotFoundError;
HPyAPI_HANDLE(52) HPy h_InterruptedError;
HPyAPI_HANDLE(53) HPy h_IsADirectoryError;
HPyAPI_HANDLE(54) HPy h_NotADirectoryError;
HPyAPI_HANDLE(55) HPy h_PermissionError;
HPyAPI_HANDLE(56) HPy h_ProcessLookupError;
HPyAPI_HANDLE(57) HPy h_TimeoutError;
// EnvironmentError, IOError and WindowsError are intentionally omitted (they
// are all aliases of OSError since Python 3.3).

/* Warnings */
HPyAPI_HANDLE(58) HPy h_Warning;
HPyAPI_HANDLE(59) HPy h_UserWarning;
HPyAPI_HANDLE(60) HPy h_DeprecationWarning;
HPyAPI_HANDLE(61) HPy h_PendingDeprecationWarning;
HPyAPI_HANDLE(62) HPy h_SyntaxWarning;
HPyAPI_HANDLE(63) HPy h_RuntimeWarning;
HPyAPI_HANDLE(64) HPy h_FutureWarning;
HPyAPI_HANDLE(65) HPy h_ImportWarning;
HPyAPI_HANDLE(66) HPy h_UnicodeWarning;
HPyAPI_HANDLE(67) HPy h_BytesWarning;
HPyAPI_HANDLE(68) HPy h_ResourceWarning;

/* Types */
HPyAPI_HANDLE(69) HPy h_BaseObjectType;   /* built-in 'object' */
HPyAPI_HANDLE(70) HPy h_TypeType;         /* built-in 'type' */
HPyAPI_HANDLE(71) HPy h_BoolType;         /* built-in 'bool' */
HPyAPI_HANDLE(72) HPy h_LongType;         /* built-in 'int' */
HPyAPI_HANDLE(73) HPy h_FloatType;        /* built-in 'float' */
HPyAPI_HANDLE(74) HPy h_UnicodeType;      /* built-in 'str' */
HPyAPI_HANDLE(75) HPy h_TupleType;        /* built-in 'tuple' */
HPyAPI_HANDLE(76) HPy h_ListType;         /* built-in 'list' */
HPyAPI_HANDLE(229) HPy h_ComplexType;     /* built-in 'complex' */
HPyAPI_HANDLE(230) HPy h_BytesType;       /* built-in 'bytes' */
HPyAPI_HANDLE(231) HPy h_MemoryViewType;  /* built-in 'memoryview' */
HPyAPI_HANDLE(232) HPy h_CapsuleType;     /* built-in 'capsule' */
HPyAPI_HANDLE(233) HPy h_SliceType;       /* built-in 'slice' */

#endif

HPyAPI_FUNC(77)
HPy HPyModule_Create(HPyContext *ctx, HPyModuleDef *def);
HPyAPI_FUNC(78)
HPy HPy_Dup(HPyContext *ctx, HPy h);
HPyAPI_FUNC(79)
void HPy_Close(HPyContext *ctx, HPy h);

HPyAPI_FUNC(80)
HPy HPyLong_FromLong(HPyContext *ctx, long value);
HPyAPI_FUNC(81)
HPy HPyLong_FromUnsignedLong(HPyContext *ctx, unsigned long value);
HPyAPI_FUNC(82)
HPy HPyLong_FromLongLong(HPyContext *ctx, long long v);
HPyAPI_FUNC(83)
HPy HPyLong_FromUnsignedLongLong(HPyContext *ctx, unsigned long long v);
HPyAPI_FUNC(84)
HPy HPyLong_FromSize_t(HPyContext *ctx, size_t value);
HPyAPI_FUNC(85)
HPy HPyLong_FromSsize_t(HPyContext *ctx, HPy_ssize_t value);

HPyAPI_FUNC(86)
long HPyLong_AsLong(HPyContext *ctx, HPy h);
HPyAPI_FUNC(87)
unsigned long HPyLong_AsUnsignedLong(HPyContext *ctx, HPy h);
HPyAPI_FUNC(88)
unsigned long HPyLong_AsUnsignedLongMask(HPyContext *ctx, HPy h);
HPyAPI_FUNC(89)
long long HPyLong_AsLongLong(HPyContext *ctx, HPy h);
HPyAPI_FUNC(90)
unsigned long long HPyLong_AsUnsignedLongLong(HPyContext *ctx, HPy h);
HPyAPI_FUNC(91)
unsigned long long HPyLong_AsUnsignedLongLongMask(HPyContext *ctx, HPy h);
HPyAPI_FUNC(92)
size_t HPyLong_AsSize_t(HPyContext *ctx, HPy h);
HPyAPI_FUNC(93)
HPy_ssize_t HPyLong_AsSsize_t(HPyContext *ctx, HPy h);
HPyAPI_FUNC(94)
void* HPyLong_AsVoidPtr(HPyContext *ctx, HPy h);
HPyAPI_FUNC(95)
double HPyLong_AsDouble(HPyContext *ctx, HPy h);

HPyAPI_FUNC(96)
HPy HPyFloat_FromDouble(HPyContext *ctx, double v);
HPyAPI_FUNC(97)
double HPyFloat_AsDouble(HPyContext *ctx, HPy h);

HPyAPI_FUNC(98)
HPy HPyBool_FromLong(HPyContext *ctx, long v);


/* abstract.h */
HPyAPI_FUNC(99)
HPy_ssize_t HPy_Length(HPyContext *ctx, HPy h);
HPyAPI_FUNC(248)
int HPySequence_Check(HPyContext *ctx, HPy h);

HPyAPI_FUNC(100)
int HPyNumber_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(101)
HPy HPy_Add(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(102)
HPy HPy_Subtract(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(103)
HPy HPy_Multiply(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(104)
HPy HPy_MatrixMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(105)
HPy HPy_FloorDivide(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(106)
HPy HPy_TrueDivide(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(107)
HPy HPy_Remainder(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(108)
HPy HPy_Divmod(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(109)
HPy HPy_Power(HPyContext *ctx, HPy h1, HPy h2, HPy h3);
HPyAPI_FUNC(110)
HPy HPy_Negative(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(111)
HPy HPy_Positive(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(112)
HPy HPy_Absolute(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(113)
HPy HPy_Invert(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(114)
HPy HPy_Lshift(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(115)
HPy HPy_Rshift(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(116)
HPy HPy_And(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(117)
HPy HPy_Xor(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(118)
HPy HPy_Or(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(119)
HPy HPy_Index(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(120)
HPy HPy_Long(HPyContext *ctx, HPy h1);
HPyAPI_FUNC(121)
HPy HPy_Float(HPyContext *ctx, HPy h1);

HPyAPI_FUNC(122)
HPy HPy_InPlaceAdd(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(123)
HPy HPy_InPlaceSubtract(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(124)
HPy HPy_InPlaceMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(125)
HPy HPy_InPlaceMatrixMultiply(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(126)
HPy HPy_InPlaceFloorDivide(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(127)
HPy HPy_InPlaceTrueDivide(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(128)
HPy HPy_InPlaceRemainder(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(129)
HPy HPy_InPlacePower(HPyContext *ctx, HPy h1, HPy h2, HPy h3);
HPyAPI_FUNC(130)
HPy HPy_InPlaceLshift(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(131)
HPy HPy_InPlaceRshift(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(132)
HPy HPy_InPlaceAnd(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(133)
HPy HPy_InPlaceXor(HPyContext *ctx, HPy h1, HPy h2);
HPyAPI_FUNC(134)
HPy HPy_InPlaceOr(HPyContext *ctx, HPy h1, HPy h2);

HPyAPI_FUNC(135)
int HPyCallable_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(136)
HPy HPy_CallTupleDict(HPyContext *ctx, HPy callable, HPy args, HPy kw);

/* pyerrors.h */
HPyAPI_FUNC(137)
void HPy_FatalError(HPyContext *ctx, const char *message);
HPyAPI_FUNC(138)
HPy HPyErr_SetString(HPyContext *ctx, HPy h_type, const char *message);
HPyAPI_FUNC(139)
HPy HPyErr_SetObject(HPyContext *ctx, HPy h_type, HPy h_value);
/* note: the filename will be FS decoded */
HPyAPI_FUNC(140)
HPy HPyErr_SetFromErrnoWithFilename(HPyContext *ctx, HPy h_type, const char *filename_fsencoded);
HPyAPI_FUNC(141)
HPy HPyErr_SetFromErrnoWithFilenameObjects(HPyContext *ctx, HPy h_type, HPy filename1, HPy filename2);
/* note: HPyErr_Occurred() returns a flag 0-or-1, instead of a 'PyObject *' */
HPyAPI_FUNC(142)
int HPyErr_Occurred(HPyContext *ctx);
HPyAPI_FUNC(143)
int HPyErr_ExceptionMatches(HPyContext *ctx, HPy exc);
HPyAPI_FUNC(144)
HPy HPyErr_NoMemory(HPyContext *ctx);
HPyAPI_FUNC(145)
void HPyErr_Clear(HPyContext *ctx);
HPyAPI_FUNC(146)
HPy HPyErr_NewException(HPyContext *ctx, const char *name, HPy base, HPy dict);
HPyAPI_FUNC(147)
HPy HPyErr_NewExceptionWithDoc(HPyContext *ctx, const char *name, const char *doc, HPy base, HPy dict);
HPyAPI_FUNC(148)
int HPyErr_WarnEx(HPyContext *ctx, HPy category, const char *message, HPy_ssize_t stack_level);
HPyAPI_FUNC(149)
void HPyErr_WriteUnraisable(HPyContext *ctx, HPy obj);

/* object.h */
HPyAPI_FUNC(150)
int HPy_IsTrue(HPyContext *ctx, HPy h);
HPyAPI_FUNC(151)
HPy HPyType_FromSpec(HPyContext *ctx, HPyType_Spec *spec,
                     HPyType_SpecParam *params);
HPyAPI_FUNC(152)
HPy HPyType_GenericNew(HPyContext *ctx, HPy type, HPy *args, HPy_ssize_t nargs, HPy kw);

HPyAPI_FUNC(153)
HPy HPy_GetAttr(HPyContext *ctx, HPy obj, HPy name);
HPyAPI_FUNC(154)
HPy HPy_GetAttr_s(HPyContext *ctx, HPy obj, const char *name);

HPyAPI_FUNC(234)
HPy HPy_MaybeGetAttr_s(HPyContext *ctx, HPy obj, const char *name);

HPyAPI_FUNC(155)
int HPy_HasAttr(HPyContext *ctx, HPy obj, HPy name);
HPyAPI_FUNC(156)
int HPy_HasAttr_s(HPyContext *ctx, HPy obj, const char *name);

HPyAPI_FUNC(157)
int HPy_SetAttr(HPyContext *ctx, HPy obj, HPy name, HPy value);
HPyAPI_FUNC(158)
int HPy_SetAttr_s(HPyContext *ctx, HPy obj, const char *name, HPy value);

HPyAPI_FUNC(159)
HPy HPy_GetItem(HPyContext *ctx, HPy obj, HPy key);
HPyAPI_FUNC(160)
HPy HPy_GetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx);
HPyAPI_FUNC(161)
HPy HPy_GetItem_s(HPyContext *ctx, HPy obj, const char *key);

HPyAPI_FUNC(162)
int HPy_Contains(HPyContext *ctx, HPy container, HPy key);

HPyAPI_FUNC(163)
int HPy_SetItem(HPyContext *ctx, HPy obj, HPy key, HPy value);
HPyAPI_FUNC(164)
int HPy_SetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx, HPy value);
HPyAPI_FUNC(165)
int HPy_SetItem_s(HPyContext *ctx, HPy obj, const char *key, HPy value);

HPyAPI_FUNC(166)
HPy HPy_Type(HPyContext *ctx, HPy obj);
// WARNING: HPy_TypeCheck/HPyType_IsSubtype could be tweaked/removed in the future,
// see issue #160
HPyAPI_FUNC(167)
int HPy_TypeCheck(HPyContext *ctx, HPy obj, HPy type);
HPyAPI_FUNC(249)
int HPy_SetType(HPyContext *ctx, HPy obj, HPy type);
HPyAPI_FUNC(250)
int HPyType_IsSubtype(HPyContext *ctx, HPy sub, HPy type);
HPyAPI_FUNC(251)
const char *HPyType_GetName(HPyContext *ctx, HPy type);

HPyAPI_FUNC(168)
int HPy_Is(HPyContext *ctx, HPy obj, HPy other);

HPyAPI_FUNC(169)
void* HPy_AsStruct(HPyContext *ctx, HPy h);
HPyAPI_FUNC(170)
void* HPy_AsStructLegacy(HPyContext *ctx, HPy h);

HPyAPI_FUNC(171)
HPy _HPy_New(HPyContext *ctx, HPy h_type, void **data);

HPyAPI_FUNC(172)
HPy HPy_Repr(HPyContext *ctx, HPy obj);
HPyAPI_FUNC(173)
HPy HPy_Str(HPyContext *ctx, HPy obj);
HPyAPI_FUNC(174)
HPy HPy_ASCII(HPyContext *ctx, HPy obj);
HPyAPI_FUNC(175)
HPy HPy_Bytes(HPyContext *ctx, HPy obj);

HPyAPI_FUNC(176)
HPy HPy_RichCompare(HPyContext *ctx, HPy v, HPy w, int op);
HPyAPI_FUNC(177)
int HPy_RichCompareBool(HPyContext *ctx, HPy v, HPy w, int op);

HPyAPI_FUNC(178)
HPy_hash_t HPy_Hash(HPyContext *ctx, HPy obj);

HPyAPI_FUNC(252)
HPy HPySeqIter_New(HPyContext *ctx, HPy seq);

/* bytesobject.h */
HPyAPI_FUNC(179)
int HPyBytes_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(180)
HPy_ssize_t HPyBytes_Size(HPyContext *ctx, HPy h);
HPyAPI_FUNC(181)
HPy_ssize_t HPyBytes_GET_SIZE(HPyContext *ctx, HPy h);
HPyAPI_FUNC(182)
char* HPyBytes_AsString(HPyContext *ctx, HPy h);
HPyAPI_FUNC(183)
char* HPyBytes_AS_STRING(HPyContext *ctx, HPy h);
HPyAPI_FUNC(184)
HPy HPyBytes_FromString(HPyContext *ctx, const char *v);
HPyAPI_FUNC(185)
HPy HPyBytes_FromStringAndSize(HPyContext *ctx, const char *v, HPy_ssize_t len);

/* unicodeobject.h */
HPyAPI_FUNC(186)
HPy HPyUnicode_FromString(HPyContext *ctx, const char *utf8);
HPyAPI_FUNC(187)
int HPyUnicode_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(188)
HPy HPyUnicode_AsASCIIString(HPyContext *ctx, HPy h);
HPyAPI_FUNC(189)
HPy HPyUnicode_AsLatin1String(HPyContext *ctx, HPy h);
HPyAPI_FUNC(190)
HPy HPyUnicode_AsUTF8String(HPyContext *ctx, HPy h);
HPyAPI_FUNC(191)
const char* HPyUnicode_AsUTF8AndSize(HPyContext *ctx, HPy h, HPy_ssize_t *size);
HPyAPI_FUNC(192)
HPy HPyUnicode_FromWideChar(HPyContext *ctx, const wchar_t *w, HPy_ssize_t size);
HPyAPI_FUNC(193)
HPy HPyUnicode_DecodeFSDefault(HPyContext *ctx, const char* v);
HPyAPI_FUNC(194)
HPy HPyUnicode_DecodeFSDefaultAndSize(HPyContext *ctx, const char* v, HPy_ssize_t size);
HPyAPI_FUNC(195)
HPy HPyUnicode_EncodeFSDefault(HPyContext *ctx, HPy h);
HPyAPI_FUNC(196)
HPy_UCS4 HPyUnicode_ReadChar(HPyContext *ctx, HPy h, HPy_ssize_t index);
HPyAPI_FUNC(197)
HPy HPyUnicode_DecodeASCII(HPyContext *ctx, const char *s, HPy_ssize_t size, const char *errors);
HPyAPI_FUNC(198)
HPy HPyUnicode_DecodeLatin1(HPyContext *ctx, const char *s, HPy_ssize_t size, const char *errors);
HPyAPI_FUNC(243)
HPy HPyUnicode_FromEncodedObject(HPyContext *ctx, HPy obj, const char *encoding, const char *errors);
HPyAPI_FUNC(244)
HPy HPyUnicode_InternFromString(HPyContext *ctx, const char *str);
HPyAPI_FUNC(245)
HPy HPyUnicode_Substring(HPyContext *ctx, HPy obj, HPy_ssize_t start, HPy_ssize_t end);

/* listobject.h */
HPyAPI_FUNC(199)
int HPyList_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(200)
HPy HPyList_New(HPyContext *ctx, HPy_ssize_t len);
HPyAPI_FUNC(201)
int HPyList_Append(HPyContext *ctx, HPy h_list, HPy h_item);

/* dictobject.h */
HPyAPI_FUNC(202)
int HPyDict_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(203)
HPy HPyDict_New(HPyContext *ctx);
HPyAPI_FUNC(246)
HPy HPyDict_Keys(HPyContext *ctx, HPy h);
/* HPyDict_GetItem

   In contrast to HPy_GetItem, this function ignores the error context. I.e.
   if 'HPyErr_Occurred(ctx) != 0', it will still work.
   This function, of course, returns a new reference.
 */
HPyAPI_FUNC(247)
HPy HPyDict_GetItem(HPyContext *ctx, HPy op, HPy key);

/* tupleobject.h */
HPyAPI_FUNC(204)
int HPyTuple_Check(HPyContext *ctx, HPy h);
HPyAPI_FUNC(205)
HPy HPyTuple_FromArray(HPyContext *ctx, HPy items[], HPy_ssize_t n);
// note: HPyTuple_Pack is implemented as a macro in common/macros.h

/* slice */
HPyAPI_FUNC(235)
int HPySlice_Unpack(HPyContext *ctx, HPy slice, HPy_ssize_t *start, HPy_ssize_t *stop, HPy_ssize_t *step);

/* contextvar */
HPyAPI_FUNC(236)
HPy HPyContextVar_New(HPyContext *ctx, const char *name, HPy default_value);
HPyAPI_FUNC(237)
int HPyContextVar_Get(HPyContext *ctx, HPy context_var, HPy default_value, HPy *result);
HPyAPI_FUNC(238)
HPy HPyContextVar_Set(HPyContext *ctx, HPy context_var, HPy value);

/* import.h */
HPyAPI_FUNC(206)
HPy HPyImport_ImportModule(HPyContext *ctx, const char *name);

/* pycapsule.h */
HPyAPI_FUNC(239)
HPy HPyCapsule_New(HPyContext *ctx, void *pointer, const char *name, HPyCapsule_Destructor destructor);
HPyAPI_FUNC(240)
void* HPyCapsule_Get(HPyContext *ctx, HPy capsule, _HPyCapsule_key key, const char *name);
HPyAPI_FUNC(241)
int HPyCapsule_IsValid(HPyContext *ctx, HPy capsule, const char *name);
HPyAPI_FUNC(242)
int HPyCapsule_Set(HPyContext *ctx, HPy capsule, _HPyCapsule_key key, void *value);

/* integration with the old CPython API */
HPyAPI_FUNC(207)
HPy HPy_FromPyObject(HPyContext *ctx, cpy_PyObject *obj);
HPyAPI_FUNC(208)
cpy_PyObject *HPy_AsPyObject(HPyContext *ctx, HPy h);

/* internal helpers which need to be exposed to modules for practical reasons :( */
HPyAPI_FUNC(209)
void _HPy_CallRealFunctionFromTrampoline(HPyContext *ctx,
                                         HPyFunc_Signature sig,
                                         HPyCFunction func,
                                         void *args);

/* Builders */

HPyAPI_FUNC(210)
HPyListBuilder HPyListBuilder_New(HPyContext *ctx, HPy_ssize_t initial_size);
HPyAPI_FUNC(211)
void HPyListBuilder_Set(HPyContext *ctx, HPyListBuilder builder,
                        HPy_ssize_t index, HPy h_item);
HPyAPI_FUNC(212)
HPy HPyListBuilder_Build(HPyContext *ctx, HPyListBuilder builder);
HPyAPI_FUNC(213)
void HPyListBuilder_Cancel(HPyContext *ctx, HPyListBuilder builder);

HPyAPI_FUNC(214)
HPyTupleBuilder HPyTupleBuilder_New(HPyContext *ctx, HPy_ssize_t initial_size);
HPyAPI_FUNC(215)
void HPyTupleBuilder_Set(HPyContext *ctx, HPyTupleBuilder builder,
                         HPy_ssize_t index, HPy h_item);
HPyAPI_FUNC(216)
HPy HPyTupleBuilder_Build(HPyContext *ctx, HPyTupleBuilder builder);
HPyAPI_FUNC(217)
void HPyTupleBuilder_Cancel(HPyContext *ctx, HPyTupleBuilder builder);

/* Helper for correctly closing handles */

HPyAPI_FUNC(218)
HPyTracker HPyTracker_New(HPyContext *ctx, HPy_ssize_t size);
HPyAPI_FUNC(219)
int HPyTracker_Add(HPyContext *ctx, HPyTracker ht, HPy h);
HPyAPI_FUNC(220)
void HPyTracker_ForgetAll(HPyContext *ctx, HPyTracker ht);
HPyAPI_FUNC(221)
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
HPyAPI_FUNC(222)
void HPyField_Store(HPyContext *ctx, HPy target_object, HPyField *target_field, HPy h);
HPyAPI_FUNC(223)
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
HPyAPI_FUNC(224)
void HPy_ReenterPythonExecution(HPyContext *ctx, HPyThreadState state);
HPyAPI_FUNC(225)
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
HPyAPI_FUNC(226)
void HPyGlobal_Store(HPyContext *ctx, HPyGlobal *global, HPy h);
HPyAPI_FUNC(227)
HPy HPyGlobal_Load(HPyContext *ctx, HPyGlobal global);

/* Debugging helpers */
HPyAPI_FUNC(228)
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
    HPy_tp_call = SLOT(50, HPyFunc_KEYWORDS),
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
    HPy_tp_iter = SLOT(62, HPyFunc_GETITERFUNC),
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

} HPySlot_Slot;

// TODO: custom enum to allow only some slots?
HPyAPI_FUNC(253)
int HPyType_CheckSlot(HPyContext *ctx, HPy type, HPyDef *value);
