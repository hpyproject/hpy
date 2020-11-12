#include <Python.h>
#include <stdlib.h>
#include "hpy.h"
#include "handles.h"


#define hpy_assert(condition, ...)                            \
    do {                                                      \
        if (!(condition)) {                                   \
            fprintf(stderr, "*** HPY HANDLES ERROR ***\n");   \
            fprintf(stderr, __VA_ARGS__);                     \
            fprintf(stderr, "\n");                            \
            abort();                                          \
        }                                                     \
    } while (0)


/* XXX we should turn the fast-path of _py2h and _h2py into macros in api.h */

static PyObject **all_handles;
static Py_ssize_t h_num_allocated = 0;
static Py_ssize_t h_free_list = -1;
static Py_ssize_t h_free_list_2 = -1;
/* note: h_free_list_2 is only here for debugging.  We can push freed
   handles directly into h_free_list.  But using two free lists makes
   reuses occur later and in a less deterministic order. */

static void
allocate_more_handles(void)
{
    if (h_free_list_2 >= 0) {
        h_free_list = h_free_list_2;
        h_free_list_2 = -1;
        return;
    }

    Py_ssize_t base = (h_num_allocated < CONSTANT_H__TOTAL ?
                       CONSTANT_H__TOTAL : h_num_allocated);
    Py_ssize_t i, allocate = (base / 2) * 3 + 32;
    PyObject **new_handles = PyMem_Malloc(sizeof(PyObject *) * allocate);
    memcpy(new_handles, all_handles, sizeof(PyObject *) * h_num_allocated);

    for (i = allocate - 1; i >= base; i--) {
        new_handles[i] = (PyObject *)((h_free_list << 1) | 1);
        h_free_list = i;
    }

    if (h_num_allocated == 0) {
        new_handles[CONSTANT_H_NULL] = NULL;
        /* Constants */
        new_handles[CONSTANT_H_NONE] = Py_None;
        new_handles[CONSTANT_H_TRUE] = Py_True;
        new_handles[CONSTANT_H_FALSE] = Py_False;
        /* Exceptions */
        new_handles[CONSTANT_H_BASEEXCEPTION] = PyExc_BaseException;
        new_handles[CONSTANT_H_EXCEPTION] = PyExc_Exception;
        new_handles[CONSTANT_H_STOPASYNCITERATION] = PyExc_StopAsyncIteration;
        new_handles[CONSTANT_H_STOPITERATION] = PyExc_StopIteration;
        new_handles[CONSTANT_H_GENERATOREXIT] = PyExc_GeneratorExit;
        new_handles[CONSTANT_H_ARITHMETICERROR] = PyExc_ArithmeticError;
        new_handles[CONSTANT_H_LOOKUPERROR] = PyExc_LookupError;
        new_handles[CONSTANT_H_ASSERTIONERROR] = PyExc_AssertionError;
        new_handles[CONSTANT_H_ATTRIBUTEERROR] = PyExc_AttributeError;
        new_handles[CONSTANT_H_BUFFERERROR] = PyExc_BufferError;
        new_handles[CONSTANT_H_EOFERROR] = PyExc_EOFError;
        new_handles[CONSTANT_H_FLOATINGPOINTERROR] = PyExc_FloatingPointError;
        new_handles[CONSTANT_H_OSERROR] = PyExc_OSError;
        new_handles[CONSTANT_H_IMPORTERROR] = PyExc_ImportError;
        new_handles[CONSTANT_H_MODULENOTFOUNDERROR] = PyExc_ModuleNotFoundError;
        new_handles[CONSTANT_H_INDEXERROR] = PyExc_IndexError;
        new_handles[CONSTANT_H_KEYERROR] = PyExc_KeyError;
        new_handles[CONSTANT_H_KEYBOARDINTERRUPT] = PyExc_KeyboardInterrupt;
        new_handles[CONSTANT_H_MEMORYERROR] = PyExc_MemoryError;
        new_handles[CONSTANT_H_NAMEERROR] = PyExc_NameError;
        new_handles[CONSTANT_H_OVERFLOWERROR] = PyExc_OverflowError;
        new_handles[CONSTANT_H_RUNTIMEERROR] = PyExc_RuntimeError;
        new_handles[CONSTANT_H_RECURSIONERROR] = PyExc_RecursionError;
        new_handles[CONSTANT_H_NOTIMPLEMENTEDERROR] = PyExc_NotImplementedError;
        new_handles[CONSTANT_H_SYNTAXERROR] = PyExc_SyntaxError;
        new_handles[CONSTANT_H_INDENTATIONERROR] = PyExc_IndentationError;
        new_handles[CONSTANT_H_TABERROR] = PyExc_TabError;
        new_handles[CONSTANT_H_REFERENCEERROR] = PyExc_ReferenceError;
        new_handles[CONSTANT_H_SYSTEMERROR] = PyExc_SystemError;
        new_handles[CONSTANT_H_SYSTEMEXIT] = PyExc_SystemExit;
        new_handles[CONSTANT_H_TYPEERROR] = PyExc_TypeError;
        new_handles[CONSTANT_H_UNBOUNDLOCALERROR] = PyExc_UnboundLocalError;
        new_handles[CONSTANT_H_UNICODEERROR] = PyExc_UnicodeError;
        new_handles[CONSTANT_H_UNICODEENCODEERROR] = PyExc_UnicodeEncodeError;
        new_handles[CONSTANT_H_UNICODEDECODEERROR] = PyExc_UnicodeDecodeError;
        new_handles[CONSTANT_H_UNICODETRANSLATEERROR] = PyExc_UnicodeTranslateError;
        new_handles[CONSTANT_H_VALUEERROR] = PyExc_ValueError;
        new_handles[CONSTANT_H_ZERODIVISIONERROR] = PyExc_ZeroDivisionError;
        new_handles[CONSTANT_H_BLOCKINGIOERROR] = PyExc_BlockingIOError;
        new_handles[CONSTANT_H_BROKENPIPEERROR] = PyExc_BrokenPipeError;
        new_handles[CONSTANT_H_CHILDPROCESSERROR] = PyExc_ChildProcessError;
        new_handles[CONSTANT_H_CONNECTIONERROR] = PyExc_ConnectionError;
        new_handles[CONSTANT_H_CONNECTIONABORTEDERROR] = PyExc_ConnectionAbortedError;
        new_handles[CONSTANT_H_CONNECTIONREFUSEDERROR] = PyExc_ConnectionRefusedError;
        new_handles[CONSTANT_H_CONNECTIONRESETERROR] = PyExc_ConnectionResetError;
        new_handles[CONSTANT_H_FILEEXISTSERROR] = PyExc_FileExistsError;
        new_handles[CONSTANT_H_FILENOTFOUNDERROR] = PyExc_FileNotFoundError;
        new_handles[CONSTANT_H_INTERRUPTEDERROR] = PyExc_InterruptedError;
        new_handles[CONSTANT_H_ISADIRECTORYERROR] = PyExc_IsADirectoryError;
        new_handles[CONSTANT_H_NOTADIRECTORYERROR] = PyExc_NotADirectoryError;
        new_handles[CONSTANT_H_PERMISSIONERROR] = PyExc_PermissionError;
        new_handles[CONSTANT_H_PROCESSLOOKUPERROR] = PyExc_ProcessLookupError;
        new_handles[CONSTANT_H_TIMEOUTERROR] = PyExc_TimeoutError;
        /* Warnings */
        new_handles[CONSTANT_H_WARNING] = PyExc_Warning;
        new_handles[CONSTANT_H_USERWARNING] = PyExc_UserWarning;
        new_handles[CONSTANT_H_DEPRECATIONWARNING] = PyExc_DeprecationWarning;
        new_handles[CONSTANT_H_PENDINGDEPRECATIONWARNING] = PyExc_PendingDeprecationWarning;
        new_handles[CONSTANT_H_SYNTAXWARNING] = PyExc_SyntaxWarning;
        new_handles[CONSTANT_H_RUNTIMEWARNING] = PyExc_RuntimeWarning;
        new_handles[CONSTANT_H_FUTUREWARNING] = PyExc_FutureWarning;
        new_handles[CONSTANT_H_IMPORTWARNING] = PyExc_ImportWarning;
        new_handles[CONSTANT_H_UNICODEWARNING] = PyExc_UnicodeWarning;
        new_handles[CONSTANT_H_BYTESWARNING] = PyExc_BytesWarning;
        new_handles[CONSTANT_H_RESOURCEWARNING] = PyExc_ResourceWarning;
        /* Types */
        new_handles[CONSTANT_H_BASEOBJECTTYPE] = (PyObject *)&PyBaseObject_Type;
        new_handles[CONSTANT_H_TYPETYPE] = (PyObject *)&PyType_Type;
        new_handles[CONSTANT_H_LONGTYPE] = (PyObject *)&PyLong_Type;
        new_handles[CONSTANT_H_UNICODETYPE] = (PyObject *)&PyUnicode_Type;
        new_handles[CONSTANT_H_TUPLETYPE] = (PyObject *)&PyTuple_Type;
        new_handles[CONSTANT_H_LISTTYPE] = (PyObject *)&PyList_Type;
        /* Check total */
        hpy_assert(CONSTANT_H__TOTAL == 74,
            "update handles.c with the list of constants");
    }

    PyMem_Free(all_handles);
    all_handles = new_handles;
    h_num_allocated = allocate;
}

HPy
_py2h(PyObject *obj)
{
    if (obj == NULL) {
        // Return the existing copy of HPy_NULL and don't create a new
        // handle.
        return HPy_NULL;
    }
    if (h_free_list < 0) {
        allocate_more_handles();
    }
    Py_ssize_t i = h_free_list;
    h_free_list = ((Py_ssize_t)all_handles[i]) >> 1;
    all_handles[i] = obj;
    return (HPy){i};
}

PyObject *
_h2py(HPy h)
{
    hpy_assert(h._i >= 0 && h._i < h_num_allocated,
        "using an HPy containing garbage: _i = %p", (void *)h._i);
    // If HPy_IsNull(h), the h._i = 0 and the line below returns the
    // pointer attached to the 0th handle, i.e. NULL.
    PyObject *result = all_handles[h._i];
    if (h._i == 0) {
        hpy_assert(result == NULL, "handle number 0 doesn't contain NULL");
        return NULL;
    }
    hpy_assert((((Py_ssize_t)result) & 1) == 0,
        "using an HPy that was freed already (or never allocated): _i = %p",
        (void *)h._i);
    hpy_assert(result != NULL,
        "NULL PyObject unexpected in handle _i = %p", (void *)h._i);
    hpy_assert(Py_REFCNT(result) > 0,
        "bogus (freed?) PyObject found in handle _i = %p", (void *)h._i);
    return result;
}

void
_hclose(HPy h)
{
    Py_ssize_t i = h._i;
    hpy_assert(i >= 0 && i < h_num_allocated,
        "freeing an HPy containing garbage: _i = %p", (void *)i);
    hpy_assert(i != 0, "freeing HPy_NULL is not allowed");
    PyObject *old = all_handles[i];
    hpy_assert((((Py_ssize_t)old) & 1) == 0,
        "freeing an HPy that was freed already (or never allocated): _i = %p",
         (void *)i);
    hpy_assert(Py_REFCNT(old) > 0,
        "bogus PyObject found while freeing handle _i = %p", (void *)h._i);
    all_handles[i] = (PyObject *)((h_free_list_2 << 1) | 1);
    h_free_list_2 = i;
    Py_DECREF(old);
}
