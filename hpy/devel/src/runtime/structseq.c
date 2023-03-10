/**
 * Helper functions to create struct sequences.
 *
 * These are not part of the HPy context or ABI, but instead are just helpers
 * that delegate to the relevant HPy/CPython APIs.
 *
 */

#include "hpy.h"
#include "buildvalue_internal.h"

#include <string.h>
#include <stdio.h>

const char * const HPyStructSequence_UnnamedField = "_";

static HPy_ssize_t count_single_items(HPyContext *ctx, const char *fmt);
static HPy build_array(HPyContext *ctx, HPy type, const char **fmt, va_list *values, HPy_ssize_t size);
static void close_array(HPyContext *ctx, HPy_ssize_t n, HPy *arr);

HPyAPI_HELPER HPy
HPyStructSequence_NewType(HPyContext *ctx, HPyStructSequence_Desc *desc)
{
    HPy_ssize_t i;

    if (desc->name == NULL) {
        HPyErr_SetString(ctx, ctx->h_SystemError,
                "Struct sequence descriptor does not define the name field.");
        return HPy_NULL;
    }

    for (i = 0; desc->fields[i].name != NULL; i++) {
    }

#ifndef HPY_ABI_CPYTHON
    HPyTracker ht;
    HPy fields, args, kwds, defs, docstring, n_fields;
    HPy result = HPy_NULL;

    HPy collections = HPyImport_ImportModule(ctx, "collections");
    if (HPy_IsNull(collections)) {
        return HPy_NULL;
    }
    HPy namedtuple = HPy_GetAttr_s(ctx, collections, "namedtuple");
    HPy_Close(ctx, collections);
    if (HPy_IsNull(namedtuple)) {
        return HPy_NULL;
    }
    ht = HPyTracker_New(ctx, 9);
    HPyTracker_Add(ctx, ht, namedtuple);


    HPyTupleBuilder argsBuilder = HPyTupleBuilder_New(ctx, 2);
    HPyTupleBuilder fieldsBuilder = HPyTupleBuilder_New(ctx, i);
    HPyTupleBuilder defsBuilder = HPyTupleBuilder_New(ctx, i);

    HPy h_name = HPyUnicode_FromString(ctx, "tmpname");
    if (HPy_IsNull(h_name)) {
        goto error;
    }
    HPyTupleBuilder_Set(ctx, argsBuilder, 0, h_name);
    HPy_Close(ctx, h_name);

    i = 0;
    for (const char* name = desc->fields[i].name; name != NULL; name = desc->fields[++i].name) {
        HPy h_field = HPyUnicode_FromString(ctx, name);
        if (HPy_IsNull(h_field)) {
            HPyTupleBuilder_Cancel(ctx, argsBuilder);
            HPyTupleBuilder_Cancel(ctx, fieldsBuilder);
            HPyTupleBuilder_Cancel(ctx, defsBuilder);
            goto error;
        }
        HPyTupleBuilder_Set(ctx, fieldsBuilder, i, h_field);
        HPyTupleBuilder_Set(ctx, defsBuilder, i, ctx->h_None);
        HPy_Close(ctx, h_field);
    }
    fields = HPyTupleBuilder_Build(ctx, fieldsBuilder);
    if (HPy_IsNull(fields)) {
        HPyTupleBuilder_Cancel(ctx, argsBuilder);
        HPyTupleBuilder_Cancel(ctx, defsBuilder);
        goto error;
    }
    HPyTracker_Add(ctx, ht, fields);

    defs = HPyTupleBuilder_Build(ctx, defsBuilder);
    if (HPy_IsNull(defs)) {
        HPyTupleBuilder_Cancel(ctx, argsBuilder);
        goto error;
    }
    HPyTracker_Add(ctx, ht, defs);

    HPyTupleBuilder_Set(ctx, argsBuilder, 1, fields);
    args = HPyTupleBuilder_Build(ctx, argsBuilder);
    if (HPy_IsNull(args)) {
        goto error;
    }
    HPyTracker_Add(ctx, ht, args);

    kwds = HPyDict_New(ctx);
    if (HPy_IsNull(kwds)) {
        goto error;
    }
    HPyTracker_Add(ctx, ht, kwds);
    if (HPy_SetItem_s(ctx, kwds, "rename", ctx->h_True) < 0) {
        goto error;
    }
    if (HPy_SetItem_s(ctx, kwds, "defaults", defs) < 0) {
        goto error;
    }

    result = HPy_CallTupleDict(ctx, namedtuple, args, kwds);
    if (HPy_IsNull(result)) {
        goto error;
    }

    if (desc->doc) {
        docstring = HPyUnicode_FromString(ctx, desc->doc);
        if (HPy_IsNull(docstring)) {
            goto error;
        }
        HPyTracker_Add(ctx, ht, docstring);
    } else {
        docstring = ctx->h_None;
    }
    if (HPy_SetAttr_s(ctx, result, "__doc__", docstring) < 0) {
        goto error;
    }

    n_fields = HPyLong_FromSsize_t(ctx, i);
    if (HPy_IsNull(n_fields)) {
        goto error;
    }
    HPyTracker_Add(ctx, ht, n_fields);
    if (HPy_SetAttr_s(ctx, result, "n_fields", n_fields) < 0) {
        goto error;
    }

    /* Set the type name and qualname */
    const char *s = strrchr(desc->name, '.');
    char *modname;
    if (s == NULL) {
        s = desc->name;
        modname = NULL;
    }
    else {
        /* TODO: Replace this by using 'HPyUnicode_FromStringAndSize' or similar
           once available. */
        const size_t n_modname = s - desc->name;
        modname = (char *)malloc((n_modname + 1) * sizeof(char));
        strncpy(modname, desc->name, n_modname);
        modname[n_modname] = '\0';

        // now, advance by one to skip '.'
        s++;
    }

    h_name = HPyUnicode_FromString(ctx, s);
    if (HPy_IsNull(h_name)) {
        goto error;
    }
    HPyTracker_Add(ctx, ht, h_name);
    if (HPy_SetAttr_s(ctx, result, "__name__", h_name) < 0 ||
            HPy_SetAttr_s(ctx, result, "__qualname__", h_name) < 0) {
        goto error;
    }

    if (modname != NULL) {
        HPy h_modname = HPyUnicode_FromString(ctx, modname);
        free(modname);
        if (HPy_IsNull(h_modname)) {
            goto error;
        }
        HPyTracker_Add(ctx, ht, h_modname);
        if (HPy_SetAttr_s(ctx, result, "__module__", h_modname) < 0) {
            goto error;
        }
    }

    HPyTracker_Close(ctx, ht);
    return result;

 error:
    HPyTracker_Close(ctx, ht);
    HPy_Close(ctx, result);
    return HPy_NULL;
#else
    PyStructSequence_Desc d = {
        .name = desc->name,
        .doc = desc->doc,
        .fields = (PyStructSequence_Field *)desc->fields,
        .n_in_sequence = i
    };
    return _py2h((PyObject*) PyStructSequence_NewType(&d));
#endif
}

static HPy
structseq_new(HPyContext *ctx, HPy type, HPy_ssize_t nargs, HPy *args, bool args_owned)
{
    static const char *s_n_fields = "n_fields";
#ifndef HPY_ABI_CPYTHON
    if (!HPy_HasAttr_s(ctx, type, s_n_fields)) {
        if (args_owned)
            close_array(ctx, nargs, args);
        HPyErr_Clear(ctx);
        HPyErr_Format(ctx, ctx->h_TypeError,
                "object '%R' does not look like a struct sequence type ", type);
        return HPy_NULL;
    }
    HPy tuple = HPyTuple_FromArray(ctx, args, nargs);
    if (args_owned)
        close_array(ctx, nargs, args);
    if (HPy_IsNull(tuple)) {
        return HPy_NULL;
    }
    HPy result = HPy_CallTupleDict(ctx, type, tuple, HPy_NULL);
    HPy_Close(ctx, tuple);
    return result;
#else
    PyTypeObject *tp = (PyTypeObject *)_h2py(type);
    // CPython also accesses the dict directly
    PyObject *name = PyUnicode_FromStringAndSize(s_n_fields, sizeof(s_n_fields));
    PyObject *v = PyDict_GetItemWithError(tp->tp_dict, name);
    Py_DECREF(name);
    if (v == NULL && !PyErr_Occurred()) {
        goto error;
    }
    Py_ssize_t n_fields = PyLong_AsSsize_t(v);
    PyObject *seq = PyStructSequence_New(tp);
    if (seq == NULL) {
        goto error;
    }
    if (n_fields != nargs) {
        PyErr_Format(PyExc_TypeError,
                     "expected exactly %d arguments but got %d",
                     n_fields, nargs);
        return HPy_NULL;
    }

    PyObject *item;
    for (Py_ssize_t i = 0; i < nargs; i++) {
        item = _h2py(args[i]);
        if (!args_owned)
            Py_INCREF(item);
        PyStructSequence_SetItem(seq, i, item);
    }
    return _py2h(seq);

error:
    /* The error behavior seems not to be consistent. If anything goes
       wrong, we will convert it to a TypeError here. */
    PyErr_Clear();
    PyErr_Format(PyExc_TypeError,
                 "type '%s' does not look like a struct sequence type",
                 tp->tp_name);
    return HPy_NULL;
#endif
}

HPyAPI_HELPER HPy
HPyStructSequence_New(HPyContext *ctx, HPy type, HPy_ssize_t nargs, HPy *args)
{
    return structseq_new(ctx, type, nargs, args, false);
}

HPyAPI_HELPER HPy
HPyStructSequence_NewFromFormat(HPyContext *ctx, HPy type, const char *fmt, ...)
{
    va_list values;
    HPy result;
    va_start(values, fmt);
    HPy_ssize_t size = count_single_items(ctx, fmt);
    if (size < 0) {
        result = HPy_NULL;
    } else if (size == 0) {
        result = HPyStructSequence_New(ctx, type, 0, NULL);
    } else if (size == 1) {
        int owned;
        result = buildvalue_single(ctx, &fmt, &values, &owned);
        if (!owned) {
            result = HPy_Dup(ctx, result);
        }
    } else {
        result = build_array(ctx, type, &fmt, &values, size);
    }
    va_end(values);
    return result;

}

static const char ERROR_FMT[] = "bad format char '%c' in the format string passed to HPyStructSequence_NewFromFormat";

static HPy_ssize_t
count_single_items(HPyContext *ctx, const char *fmt)
{
    HPy_ssize_t i;
    char c;
    for (i = 0; (c = fmt[i]) != '\0'; i++) {
        switch (c) {
        case 'i':
        case 'I':
        case 'k':
        case 'l':
        case 'L':
        case 'K':
        case 's':
        case 'O':
        case 'S':
        case 'N':
        case 'f':
        case 'd':
            break;

        default: {
            /* the size of ERROR_FMT will be enough since we are just inserting
               a single char */
            char msg[sizeof(ERROR_FMT)];
            snprintf(msg, sizeof(msg), ERROR_FMT, c);
            HPyErr_SetString(ctx, ctx->h_SystemError, msg);
            return -1;
        }
        }
    }
    return i;
}

static void
close_array(HPyContext *ctx, HPy_ssize_t n, HPy *arr)
{
    for (HPy_ssize_t i=0; i < n; i++) {
        HPy_Close(ctx, arr[i]);
    }
}

static HPy
build_array(HPyContext *ctx, HPy type, const char **fmt, va_list *values, HPy_ssize_t size)
{
    HPy arr[size];
    for (HPy_ssize_t i = 0; i < size; ++i) {
        int owned;
        HPy item = buildvalue_single(ctx, fmt, values, &owned);
        if (HPy_IsNull(item)) {
            // in case of error, close all previously created items
            close_array(ctx, i, arr);
            return HPy_NULL;
        }
        arr[i] = owned ? item : HPy_Dup(ctx, item);
    }
    /* Sanity check: after we have consumed 'size' items, we expect the end of
       of the format string. Otherwise, function 'count_single_items' is
       not counting correctly. */
    if (**fmt != '\0') {
        close_array(ctx, size, arr);
        HPyErr_SetString(ctx, ctx->h_SystemError,
                "internal error in HPyStructSequence_NewFromFormat");
        return HPy_NULL;
    }
    return structseq_new(ctx, type, size, arr, true);
}
