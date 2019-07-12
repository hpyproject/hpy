The PyHandle API
===================

Idea: instead of using the type `PyObject *`, we use `HPy`.  It is a
"handle" to an object.  It works like Windows HANDLE or like Unix file
descriptors: it is similar to a pointer, but opaque; and most importantly two
different handles can refer to the same object (i.e. you can't use `==` to
compare them).  A possible implementation would be:

```C
typedef struct { void *_internal; } HPy;
```

After you call some API and get a HPy, you usually need to release it;
to do that, instead of calling `Py_DECREF()`, you call `HPy_Close()`
(all names subject to change).  Replacing `Py_INCREF()` is less mechanical:
you need to call `y = HPy_Dup(x);`, after which you get a new handle
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
    HPy v = HPy_Something();
    HPy w = HPy_Dup(v);   /* note that 'v != w' now! */
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
    v = HPy_Something();
    w = HPy_Dup(v);
    ...
    HPy_Close(w);   /* we need to close 'w' and 'v', not twice 'v' */
    HPy_Close(v);
```


Fast implementation on CPython
------------------------------

On CPython, the internal implementation is straightforward, and the C
compiler removes all the overhead:

```C
typedef struct { PyObject *_o; } HPy;

static inline HPy HPy_IntFromLong(long value)
{
    HPy result;
    result._o = PyInt_FromLong(value);
    return result;
}

static inline HPy HPy_Dup(HPy x)
{
    Py_INCREF(x->_o);
    return x;
}

static inline void HPy_Close(HPy x)
{
    Py_DECREF(x->_o);
}
```


Debug-mode implementation on CPython
------------------------------------

Since the semantics are slightly different than `Py_INCREF` and `Py_DECREF`, it
is a good idea to provide a debug build mode that actively checks that handles
are closed properly and not used after being closed.  This would be slower but
provide a better debugging experience---for example, handles could record the C
backtrace of where they come from, which would allow very precise error
messages.  (In this mode, handles would not be simply `PyObject *`.)


Implementation on PyPy
----------------------

On a garbage-collected VM like PyPy, it can instead be implemented
without reference counts, for example like this (this is much more
efficient than what PyPy currently needs to do):

```C
typedef struct { int _i; } HPy;

/* Array of all open handles---this is _not_ all objects!
 * The GC knows that it needs to look inside this array for objects
 * that must survive ("roots").
 */
internal_gc_object_t[] _open_handles;

static inline HPy HPy_IntFromLong(long value)
{
    HPy result;
    result._i = _get_handle_from_free_list();
    _open_handles[result._i] = make_integer_obj(value);
    return result;
}

static inline HPy HPy_Dup(HPy x)
{
    HPy result;
    result._i = _get_handle_from_free_list();
    _open_handles[result._i] = _open_handles[x._i];
    return result;
}

static inline void HPy_Close(HPy x)
{
    _open_handles[x._i] = NULL;
    _put_back_handle_into_free_list(x._i);
}
```
