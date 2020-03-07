#ifndef HPY_UNIVERSAL_TYPE_H
#define HPY_UNIVERSAL_TYPE_H


typedef struct{
    int slot;    /* slot id, see below */
    void *pfunc; /* function pointer */
} HPyType_Slot;


typedef struct {
    const char* name;
    int basicsize;
    int itemsize;
    unsigned int flags;
    HPyType_Slot *slots; /* terminated by slot==0. */
} HPyType_Spec;

#define HPyObject_HEAD

/* All types are dynamically allocated */
#define _Py_TPFLAGS_HEAPTYPE (1UL << 9)

/* Set if the type allows subclassing */
#define HPy_TPFLAGS_BASETYPE (1UL << 10)

#define HPy_TPFLAGS_DEFAULT _Py_TPFLAGS_HEAPTYPE

#endif /* HPY_UNIVERSAL_TYPE_H */
