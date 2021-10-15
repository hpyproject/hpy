Changelog
=========

Version 0.0.3 (September 22nd, 2021)
------------------------------------

This release adds various new API functions (see below) and extends the debug 
mode with the ability to track closed handles.
The default ABI mode now is 'universal' for non-CPython implementations.
Also, the type definition of `HPyContext` was changed and it's no longer a
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
