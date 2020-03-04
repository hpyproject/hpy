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
 * Decision:
 */

typedef struct {
  HPy _h;
  void * _data;
  HPy_ssize_t _length;
} HPySequence;

HPy foo;
HPySequence foo_seq = HPy_AsSequence(ctx, foo);
HPy item = HPySequence_GetItem(ctx, foo_seq, 42);
HPySequence_SetItem(ctx, foo_seq, 40, item);
HPySequence_Close(ctx, foo_seq);
HPy_Close(ctx, item);

HPy foo;
HPy idx = HPyLong_FromLong(ctx, 42);
// check errors
HPy item = HPyObject_GetItem(ctx, foo, idx);
HPy_Close(ctx, idx);
HPy_Close(ctx, item);

HPy foo;
// check errors
HPy item = HPyObject_GetItemI(ctx, foo, 42);
HPy_Close(ctx, idx);
HPy_Close(ctx, item);

/*
 * Should HPy support Python without the GIL?
 *
 * Decision: ???
 */
