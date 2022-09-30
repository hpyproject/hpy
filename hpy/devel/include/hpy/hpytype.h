#ifndef HPY_UNIVERSAL_HPYTYPE_H
#define HPY_UNIVERSAL_HPYTYPE_H

#include <stdbool.h>
#ifdef __GNUC__
#define HPyAPI_UNUSED __attribute__((unused)) static inline
#else
#define HPyAPI_UNUSED static inline
#endif /* __GNUC__ */

typedef enum {
    HPyType_BuiltinShape_Legacy = -1,
    HPyType_BuiltinShape_Object = 0,
    HPyType_BuiltinShape_Type = 1,
    HPyType_BuiltinShape_Long = 2,
    HPyType_BuiltinShape_Float = 3,
    HPyType_BuiltinShape_Unicode = 4,
    HPyType_BuiltinShape_Tuple = 5,
    HPyType_BuiltinShape_List = 6,
} HPyType_BuiltinShape;

typedef struct {
    const char* name;
    int basicsize;
    int itemsize;
    unsigned long flags;
    /*
       A type whose struct starts with PyObject_HEAD is a legacy type. A legacy
       type must set .builtin_shape = HPyType_BuiltinShape_Legacy in its
       HPyType_Spec. A type is a non-legacy type, also called HPy pure type, if
       its struct does not include PyObject_HEAD. Using pure types should be
       preferred. Legacy types are available to allow gradual porting of
       existing CPython extensions.

       A type with .legacy_slots != NULL is required to have
       HPyType_BuiltinShape_Legacy and to include PyObject_HEAD at the start of
       its struct. It would be easy to relax this requirement on CPython (where
       the PyObject_HEAD fields are always present) but a large burden on other
       implementations (e.g. PyPy, GraalPython) where a struct starting with
       PyObject_HEAD might not exist.

       Types that do not define a struct of their own, should set the value of
       .builtin_shape to the same value as the type they inherit from. If they
       inherit from a built-in type, they must set the corresponding
       .builtin_shape.

       Types created via the old Python C API are automatically legacy types.
     */
    HPyType_BuiltinShape builtin_shape;
    void *legacy_slots; // PyType_Slot *
    HPyDef **defines;   /* points to an array of 'HPyDef *' */
    const char* doc;    /* UTF-8 doc string or NULL */
} HPyType_Spec;

typedef enum {
    HPyType_SpecParam_Base = 1,
    HPyType_SpecParam_BasesTuple = 2,
    //HPyType_SpecParam_Metaclass = 3,
    //HPyType_SpecParam_Module = 4,
} HPyType_SpecParam_Kind;

typedef struct {
    HPyType_SpecParam_Kind kind;
    HPy object;
} HPyType_SpecParam;

/* All types are dynamically allocated */
#define _Py_TPFLAGS_HEAPTYPE (1UL << 9)
#define _Py_TPFLAGS_HAVE_VERSION_TAG (1UL << 18)
#define HPy_TPFLAGS_DEFAULT (_Py_TPFLAGS_HEAPTYPE | _Py_TPFLAGS_HAVE_VERSION_TAG)

/* Set if the type allows subclassing */
#define HPy_TPFLAGS_BASETYPE (1UL << 10)

/* If set, the object will be tracked by CPython's GC. Probably irrelevant for
   GC-based alternative implementations */
#define HPy_TPFLAGS_HAVE_GC (1UL << 14)

/* A macro for creating (static inline) helper functions for custom types.

   Two versions of the helper exist. One for legacy types and one for pure
   HPy types.

   Example for a pure HPy custom type:

       HPyType_HELPERS(PointObject)

   It is also possible to inherit from some built-in types. The list of
   available built-in base types is given in enum `HPyTupe_BuiltinShape`.
   In case you want to inherit from one of those, it is necessary to specify
   the base built-in type in the `HPyType_HELPERS` macro. Here is an example
   for a pure HPy custom type inheriting from a built-in type 'tuple':

       HPyType_HELPERS(PointObject, HPyType_BuiltinShape_Tuple)

   This would generate the following:

   * `PointObject * PointObject_AsStruct(HPyContext *ctx, HPy h)`: a static
     inline function that uses HPy_AsStruct to return the PointObject struct
     associated with a given handle. The behaviour is undefined if `h`
     is associated with an object that is not an instance of PointObject.

   * `PointObject_SHAPE`: an enum value set to 0 so that in the HPyType_Spec
     for PointObject one can write `.builtin_shape = PointObject_SHAPE` and not
     have to remember to update the spec when the helpers used changes.

   Example for a legacy custom type:

       HPyType_LEGACY_HELPERS(PointObject)

   This would generate the same functions and constants as above, except:

   * `_HPy_AsStruct_Legacy` is used instead of `_HPy_AsStruct_Object`.

   * `PointObject_SHAPE` is set to `HPyType_BuiltinShape_Legacy`.
*/

/* Just a few helper macros to select the appropriate argument from the
   variadic-macro's arguments. */

#define HPyType_BUILTIN_HELPERS(TYPE, SHAPE) \
    _HPyType_GENERIC_HELPERS(TYPE, SHAPE)

#define HPyType_HELPERS(TYPE) \
    _HPyType_GENERIC_HELPERS(TYPE, HPyType_BuiltinShape_Object)

#define HPyType_LEGACY_HELPERS(TYPE) \
    _HPyType_GENERIC_HELPERS(TYPE, HPyType_BuiltinShape_Legacy)

#define _HPyType_GENERIC_HELPERS(TYPE, SHAPE) \
                                              \
_HPy_UNUSED static const                      \
HPyType_BuiltinShape TYPE##_SHAPE = SHAPE;    \
                                              \
HPyAPI_UNUSED TYPE *                          \
TYPE##_AsStruct(HPyContext *ctx, HPy h)       \
{                                             \
    return (TYPE*) SHAPE##_AsStruct(ctx, h);  \
}

#define HPyType_BuiltinShape_Legacy_AsStruct _HPy_AsStruct_Legacy
#define HPyType_BuiltinShape_Object_AsStruct _HPy_AsStruct_Object
#define HPyType_BuiltinShape_Type_AsStruct _HPy_AsStruct_Type
#define HPyType_BuiltinShape_Long_AsStruct _HPy_AsStruct_Long
#define HPyType_BuiltinShape_Float_AsStruct _HPy_AsStruct_Float
#define HPyType_BuiltinShape_Unicode_AsStruct _HPy_AsStruct_Unicode
#define HPyType_BuiltinShape_Tuple_AsStruct _HPy_AsStruct_Tuple
#define HPyType_BuiltinShape_List_AsStruct _HPy_AsStruct_List

#endif /* HPY_UNIVERSAL_HPYTYPE_H */
