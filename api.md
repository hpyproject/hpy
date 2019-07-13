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
    if (v == NULL) return NULL;
    Py_INCREF(v);
    mystruct->field = v;
    return v;
```

would become this:

```C
    HPy v = HPy_Something(ctx);
    if (HPy_IsError(v)) return HPy_ERROR;
    HPy w = HPy_Dup(ctx, v);
    if (HPy_IsError(w)) { HPy_Close(ctx, v); return HPy_ERROR; }
    /* note that 'v != w' now! */
    mystruct->field = w;
    return v;
}
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
    HPy v = HPy_Something(ctx);
    HPy w = HPy_Dup(ctx, v);
    ...
    HPy_Close(ctx, w);   /* we need to close 'w' and 'v', not twice 'v' */
    HPy_Close(ctx, v);
```

The `ctx` is an opaque "context" argument that stands for the current
interpreter.  Putting it from day 1 allows for future extensions (see below).
This is vaguely modeled on the JNI (Java Native Interface).

`HPy_Close()` can't fail, like `Py_DECREF()` can't fail.  Debug-mode
implementations could still check that you're not e.g. closing the same handle
twice, but if you do they can dump the error to stderr and `abort()`.


Fast implementation on CPython
------------------------------

On CPython, the internal implementation is straightforward, and the C
compiler removes all the overhead:

```C
typedef struct { PyObject *_o; } HPy;

static inline int HPy_IntFromLong(HPyContext ctx, long value, HPy *result)
{
    result->_o = PyInt_FromLong(value);
    return result->_o != NULL ? 0 : -1;
}

static inline int HPy_Dup(HPyContext ctx, HPy x, HPy *result)
{
    Py_INCREF(x._o);
    result->_o = x._o;
    return 0;
}

static inline void HPy_Close(HPyContext ctx, HPy x)
{
    Py_DECREF(x._o);
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

static inline int HPy_IntFromLong(HPyContext ctx, long value, HPy *result)
{
    result->_i = _get_handle_from_free_list();
    _open_handles[result->_i] = make_integer_obj(value);
    return 0;   /* or -1 if we're running out of memory */
}

static inline int HPy_Dup(HPyContext ctx, HPy x, HPy *result)
{
    result->_i = _get_handle_from_free_list();
    _open_handles[result->_i] = _open_handles[x._i];
    return 0;
}

static inline void HPy_Close(HPyContext ctx, HPy x)
{
    _open_handles[x._i] = NULL;
    _put_back_handle_into_free_list(x._i);
}
```


General design guidelines
-------------------------

* we don't want returned borrowed references.  When a function returns a
  HPy, it means that you HAVE to close it

* everything should be opaque by default; we don't expose the internal layout
  of objects.  For example, `PyList_GET_ITEM()` is a macro that reads the internals
  of lists.  Also, currently, the exact layout of type objects (`PyTypeObject`) is
  part of the API; we want something similar to `PyType_FromSpec()`.


Specialized protocols
---------------------

One of the goal is to remove the abstraction overhead when possible.  For
example, consider the case of iterating over a sequence (list, tuple,
array.array, etc.) that happens to contain integers.

Design goals:

- Expose internal data representations, which might be more efficient than objects.
- Make the direct access to native data structures explicit to allow implementors to adapt without being bound to internal details.
- Keep the API similar for both simple object access and optimised data structures.

Considering the iteration protocol, which would look as follows::

    /* If the object is not a sequence, we might want to fall back to generic iteration. */
    HPySequence seq = HPy_AsSequence(ctx, obj);
    if (HPy_Sequence_IsError(seq)) goto not_a_sequence;
    HPy_Close(ctx, obj);   /* we'll be using only 'seq' in the sequel */

    Py_ssize_t len = HPy_Sequence_Len(ctx, seq);
    for(int i=0; i<len; i++) {
        /* HPy_Sequence_GetItem will check a flag on seq to see if it can use a
           fast-path of direct indexing or it needs to go through a generic
           fallback. And the C compiler will hoist the check out of the loop,
           hopefully */
        HPy item = HPy_Sequence_GetItem(ctx, seq, i);  /* like PyList_GET_ITEM */
        /* process 'item' */
        HPy_Close(ctx, item);
    }
    HPySequenceClose(ctx, seq);

    not_a_sequence:
    /* iterator = HPy_CallSpecialMethodNoArgs(ctx, obj, __iter__) */
    HPy iterator = HPy_GetIter(ctx, obj);
    HPy_Close(ctx, obj);   /* we have 'iterator' */

    while (true) {
        HPy item = HPy_IterNext(ctx, iterator);
        if (HPy_IsError(item)) goto oops;
        if (HPy_IsIterStop(item)) break;
        /* process 'item' */
        HPy_Close(ctx, item);
    }
    HPy_Close(ctx, iterator);


Optimised variant when a sequence of C long integers is expected::

    HPySequence_long seq;
    /* This is allowed to fail and you should be ready to handle the fallback. */
    if (HPy_AsSequence_long(ctx, obj, &seq) < 0) goto not_a_long_sequence;
    HPy_Close(ctx, obj);

    Py_ssize_t len;
    if (HPy_Sequence_Len_long(ctx, seq, &len) < 0) goto error_no_length;
    for(int i=0; i<len; i++) {
        long item;
        if (HPy_Sequence_GetItem_long(ctx, seq, i, &item) < 0)
            goto error_on_item_access;
    }
    HPySequenceClose_long(ctx, seq);
