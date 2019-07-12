The PyHandle API
===================

Idea: instead of using the type `PyObject *`, we use `PyHandle`.  It is a
"handle" to an object.  It works like Windows HANDLE or like Unix file
descriptors: it is similar to a pointer, but opaque; and most importantly two
different handles can refer to the same object (i.e. you can't use `==` to
compare them).  A possible implementation would be:

```C
typedef struct { void *_internal; } PyHandle;
```

After you call some API and get a PyHandle, you usually need to release it;
to do that, instead of calling `Py_DECREF()`, you call `PyHandle_Close()`
(all names subject to change).  Replacing `Py_INCREF()` is less mechanical:
you need to call `y = PyHandle_Dup(x);`, after which you get a new handle
`y` which is now independent of `x`, and both of them need to be closed
at some point in the future.

For example, this kind of code:

```C
    PyObject *v = PyObject_Something();
    Py_INCREF(v);
    mystruct->field = v;
    return v;
```

would become this:

```C
    PyHandle v = PyHandle_Something();
    PyHandle w = PyHandle_Dup(v);   /* note that 'v != w' now! */
    mystruct->field = w;
    return v;
```

Be careful that this:

```C
    v = PyObject_Something();
    Py_INCREF(v);
    ...
    Py_DECREF(v);
    Py_DECREF(v);
```

needs to be turned into:

```C
    v = PyHandle_Something();
    w = PyHandle_Dup(v);
    ...
    PyHandle_Close(w);   /* we need to close 'w' and 'v', not twice 'v' */
    PyHandle_Close(v);
```

On CPython, the internal implementation is straightforward, and the C
compiler removes all the overhead:

```C
typedef struct { PyObject *_o; } PyHandle;

static inline PyHandle PyHandle_IntFromLong(long value)
{
    PyHandle result;
    result._o = PyInt_FromLong(value);
    return result;
}

static inline PyHandle PyHandle_Dup(PyHandle x)
{
    Py_INCREF(x->_o);
    return x;
}

static inline void PyHandle_Close(PyHandle x)
{
    Py_DECREF(x->_o);
}
```

On a garbage-collected VM like PyPy, it can instead be implemented
without reference counts, for example like this (this is much more
efficient than what PyPy currently needs to do):

```C
typedef struct { int _i; } PyHandle;

/* Array of all open handles---this is _not_ all objects!
 * The GC knows that it needs to look inside this array for objects
 * that must survive ("roots").
 */
internal_gc_object_t[] _open_handles;

static inline PyHandle PyHandle_IntFromLong(long value)
{
    PyHandle result;
    result._i = _get_handle_from_free_list();
    _open_handles[result._i] = make_integer_obj(value);
    return result;
}

static inline PyHandle PyHandle_Dup(PyHandle x)
{
    PyHandle result;
    result._i = _get_handle_from_free_list();
    _open_handles[result._i] = _open_handles[x._i];
    return result;
}

static inline void PyHandle_Close(PyHandle x)
{
    _open_handles[x._i] = NULL;
    _put_back_handle_into_free_list(x._i);
}
```
