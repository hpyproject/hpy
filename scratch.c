/*
 * Should HPy_Close silently ignore attempts to close HPy_NULL or not?
 *
 * Decision: HPy_Close should function like Py_XDECREF (i.e. silently NOT
 * close HPy_NULL).
 *
 * Decision: HPy_Dup should silently not dup HPy_NULL (for symmetry with
 *            HPy_Close), i.e. HPy_Dup(ctx, HPy_NULL) should return HPy_NULL.
 *
 * Decision: Both API and ABI versions should have this behaviour.
 *
 * Decision: We should add tests for this behaviour.
 */

/*
 * Should we have separate HPySequence_GetItem and HPyMapping_GetItem
 * or just one HPy_GetItem?
 *
 * Explore the idea of "protocols" and whether we can use it for things like
 * PyDict_GetItem (hodgestar, antocuni, ...)
 *
 * Decision: We should start with HPy_GetItem and HPy_SetItem and
 *           HPy_GetItem_i and HPy_SetItem_i. And HPy_GetAttr, HPy_SetAttr,
 *           HPy_GetAttr_s and HPy_SetAttr_s.
 *
 * Decision: We could add a hpy/compat.h that supports accessing
 *           the old Python mapping and sequence get item slots.
 *           For example, HPyDict_GetItem_NotBorrowed.
 *
 *           PyDict_GetItem returns a borrower reference.
 *           HPyDict_GetItem_NotBorrowed should return a new handle that has
 *           to be closed.
 *
 * Decision: HPy API functions MUST NOT return a borrowed reference (add to
 *           official documentation).
 *
 * Decision: We should add an HPyBuffer protocol (design to be decided).
 *
 */

HPy HPy_GetItem(HPyContext ctx, HPy obj, HPy idx);
HPy HPy_GetItem_i(HPyContext ctx, HPy obj, HPy_ssize_t idx);
HPy HPy_GetItem_s(HPyContext ctx, HPy obj, const char * idx); // UTF8 bytes

int HPy_SetItem(HPyContext ctx, HPy obj, HPy idx, HPy value);
int HPy_SetItem_i(HPyContext ctx, HPy obj, HPy_ssize_t idx, HPy value);
int HPy_SetItem_s(HPyContext ctx, HPy obj, const char * idx, HPy value); // UTF8 bytes

HPy HPy_GetAttr(HPyContext ctx, HPy obj, HPy idx);
HPy HPy_GetAttr_s(HPyContext ctx, HPy obj, const char * idx); // UTF8 bytes

int HPy_SetAttr(HPyContext ctx, HPy obj, HPy idx, HPy value);
int HPy_SetAttr_s(HPyContext ctx, HPy obj, const char * idx, HPy value); // UTF8 bytes

/*
 * Should we have an HPy_RETURN_NONE macro?
 *
 * Decision: Yes we should, but it should be HPy_RETURN_NONE(ctx).
 *
 * Decision: We should also change HPy_IsNull(x) to HPy_IsNull(ctx, x).
 *
 */

#define HPy_RETURN_NONE(ctx) return HPy_Dup(ctx, ctx->h_None);

#define HPy_IsNull(ctx, x) ...;

/*
 * Should we add HPy_AsVoidP and HPy_FromVoidP to the API? Should they be on
 * the ctx or not?
 *
 * Decision: Yes, they should be part of the API.
 *
 * Decision: They should take the ctx as an argument in case a future
 *           implementation needs it (much like HPy_IsNull).
 */

// universal
static inline HPy HPy_FromVoidP(HPyContext ctx, void *p) { return (HPy){(HPy_ssize_t)p}; }
static inline void* HPy_AsVoidP(HPyContext ctx, HPy h) { return (void*)h._i; }

// cpython (the -/+4 is to avoid people casting it to PyObject)
static inline HPy HPy_FromVoidP(HPyContext ctx, void *p) { return (HPy){(HPy_ssize_t)p - 4}; }
static inline void* HPy_AsVoidP(HPyContext ctx, HPy h) { return (void*)h._o + 4; }

/*
 * Should we implement HPy_Dump?
 *
 * Question: What should it print and where should it print it?
 *
 * Decision: It's useful for debugging if all macros are also available as functions
 *           definitions.
 *
 * Decision: It should dump to stderr just like PyObject_Dump.
 */

/*
 * How do we silence warnings from using HPy_METH_KEYWORDS?
 *
 * Decision: Write a cast to HPyMeth.
 */

{"add_ints_kw", (HPyMeth) add_ints_kw, HPy_METH_KEYWORDS, ""}

/*
 * How should HPyErr_Format be implemented? Should we avoid va_args?
 *
 * Decision:
 */

HPyErr_Format(ctx, const char *fmt, ...) {
  const char *msg = HPyStr_Format(ctx, fmt, ...);
  HPyErr_SetString(msg);
}

ctx->ctx_HPyErr_Format(???)

/*
 * Should make specifying values for optional arguments & dup / closing them
 * less messy?
 *
 * Decision: Right now, No. In the future, someone should invent
 *           ARGUMENT_CLINIC for HPy.
 */

/*
 * Should HPy support Python without the GIL?
 *
 * Decision: ???
 */
