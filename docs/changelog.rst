Changelog
=========

Version 0.9 (April 21st, 2023)
------------------------------

New Features/API

* Python 3.11 support
* Preliminary Python 3.12 support
* Dropped Python 3.6 support (since EOL)
* Dropped Python 3.7 support (since EOL by June 2023)
* Ensure deterministic auto-generation
* Ensure ABI backwards compatibility

  * Explicitly define slot within HPyContext of function pointers and handles
  * Compile HPy ABI version into binary and verify at load time
* Added support for backtrace on MacOS
* Added proper support for object members ``HPyMember_OBJECT``
* Support subclasses of built-in types
* Support for metaclasses
* Simplified ``HPyDef_*`` macros
* Introduced **Hybrid ABI**
* Introduced **Trace Mode**
* Changed ``HPyBytes_(AsString/AS_STRING)`` to return ``const char *``
* Use fixed-width integers in context functions
* Support multi-phase module initialization
* HPy call protocol

  * Enabled slot ``HPy_tp_call``
  * Added ``HPy_SetCallFunction`` to allow setting per-object call function
  * Slightly changed signature of ``HPyFunc_(VARARGS/KEYWORDS)``
* Other added API functions

  * Deleting attributes and items ``HPy_DelAttr(_s)``, ``HPy_DelItem(_i/_s)``
  * Capsule API ``HPyCapsule_(New/Get*/IsValid/Set*)``
  * Eval API ``HPy_Compile_s`` and ``HPy_EvalCode``
  * Formatting helpers ``HPyUnicode_FromFormat`` and ``HPyErr_Format``
  * Contextvar API ``HPyContextVar_(New/Get/Set)``
  * Unicode API ``HPyUnicode_FromEncodedObject`` and ``HPyUnicode_Substring``
  * Dict API ``HPyDict_Keys`` and ``HPyDict_Copy``
  * Type API ``HPyType_GetName`` and ``HPyType_IsSubtype``
  * Slice API ``HPySlice_Unpack`` and ``HPySlice_AdjustIndices``
  * Structseq API ``HPyStructSequence_New(Type)``
  * Call API ``HPy_Call(Method)`` and ``HPy_CallMethodTupleDict(_s)``

* Documentation
  * Added incremental porting example
  * Added quickstart guide
  * Extended API reference a lot
  * Added API function index
  * Added possiblity to generate examples from tests with argument ``--dump-dir``
* Debug mode
  * Detect closing and returning (without dup) of context handles
  * Detect invalid usage of stored HPyContext pointer



Version 0.0.4 (May 25th, 2022)
------------------------------

New Features/API:

  - HPy headers are C++ compliant
  - Python 3.10 support
  - `HPyField <https://github.com/hpyproject/hpy/blob/master/hpy/tools/autogen/public_api.h#L323>`_:
    References to Python objects that can be stored in raw native memory owned by Python objects.

    - New API functions: ``HPyField_Load``, ``HPyField_Store``
  - `HPyGlobal <https://github.com/hpyproject/hpy/blob/master/hpy/tools/autogen/public_api.h#L383>`_:
    References to Python objects that can be stored into a C global variable.

    - New API functions: ``HPyGlobal_Load``, ``HPyGlobal_Store``
    - Note: ``HPyGlobal`` does not allow to share Python objects between (sub)interpreters

  - `GIL support <https://github.com/hpyproject/hpy/blob/master/hpy/tools/autogen/public_api.h#L358>`_
    - New API functions: ``HPy_ReenterPythonExecution``, ``HPy_LeavePythonExecution``

  - `Value building support <https://github.com/hpyproject/hpy/blob/master/hpy/devel/src/runtime/buildvalue.c#L4>`_ (``HPy_BuildValue``)

  - New type slots

    - ``HPy_mp_ass_subscript``, ``HPy_mp_length``, ``HPy_mp_subscript``
    - ``HPy_tp_finalize``

  - Other new API functions

    - ``HPyErr_SetFromErrnoWithFilename``, ``HPyErr_SetFromErrnoWithFilenameObjects``
    - ``HPyErr_ExceptionMatches``
    - ``HPyErr_WarnEx``
    - ``HPyErr_WriteUnraisable``
    - ``HPy_Contains``
    - ``HPyLong_AsVoidPtr``
    - ``HPyLong_AsDouble``
    - ``HPyUnicode_AsASCIIString``, ``HPyUnicode_DecodeASCII``
    - ``HPyUnicode_AsLatin1String``, ``HPyUnicode_DecodeLatin1``
    - ``HPyUnicode_DecodeFSDefault``, ``HPyUnicode_DecodeFSDefaultAndSize``
    - ``HPyUnicode_ReadChar``

Debug mode:

  - Support activation of debug mode via environment variable ``HPY_DEBUG``
  - Support capturing stack traces of handle allocations
  - Check for invalid use of raw data pointers (e.g ``HPyUnicode_AsUTF8AndSize``) after handle was closed.
  - Detect invalid handles returned from extension functions
  - Detect incorrect closing of handles passed as arguments

Misc Changes:

  - Removed unnecessary prefix ``"m_"`` from fields of ``HPyModuleDef`` (incompatible change)
  - For HPy implementors: new pytest mark for HPy tests assuming synchronous GC

Version 0.0.3 (September 22nd, 2021)
------------------------------------

This release adds various new API functions (see below) and extends the debug
mode with the ability to track closed handles.
The default ABI mode now is 'universal' for non-CPython implementations.
Also, the type definition of ``HPyContext`` was changed and it's no longer a
pointer type.
The name of the HPy dev package was changed to 'hpy' (formerly: 'hpy.devel').
Macro HPy_CAST was replaced by HPy_AsStruct.

New features:

  - Added helper HPyHelpers_AddType for creating new types
  - Support format specifier 's' in HPyArg_Parse
  - Added API functions: HPy_Is, HPy_AsStructLegacy (for legacy types),
    HPyBytes_FromStringAndSize, HPyErr_NewException, HPyErr_NewExceptionWithDoc,
    HPyUnicode_AsUTF8AndSize, HPyUnicode_DecodeFSDefault, HPyImport_ImportModule
  - Debug mode: Implemented tracking of closed handles
  - Debug mode: Add hook for invalid handle access

Bug fixes:

  - Distinguish between pure and legacy types
  - Fix Sphinx doc errors
