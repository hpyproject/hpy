#define STRINGIFY(X) #X
#define HPy_ID(X) _Pragma(STRINGIFY(id=X)) \

#define AUTOGEN 1

// These are not real typedefs: they are there only to make pycparser happy
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
typedef int HPyType_BuiltinShape;

#include "public_api.h"
