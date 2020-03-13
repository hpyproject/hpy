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


#endif /* HPY_UNIVERSAL_TYPE_H */
