// BEGIN: foo
void iterate_objects()
{
    /* If the object is not a sequence, we might want to fall back to generic iteration. */
    HPySequence seq = HPy_AsSequence(ctx, obj);
    if (HPy_Sequence_IsError(seq))
        goto not_a_sequence;
    HPy_Close(ctx, obj);   /* we'll be using only 'seq' in the sequel */
    HPy_ssize_t len = HPy_Sequence_Len(ctx, seq);
    for (int i=0; i<len; i++)
    {
        /* HPy_Sequence_GetItem will check a flag on seq to see if it can use a
           fast-path of direct indexing or it needs to go through a generic
           fallback. And the C compiler will hoist the check out of the loop,
           hopefully */
        HPy item = HPy_Sequence_GetItem(ctx, seq, i); /* like PyList_GET_ITEM */

        /* process 'item' */
        HPy_Close(ctx, item);
    }
    HPySequenceClose(ctx, seq);

not_a_sequence:
    HPy iterator = HPy_GetIter(ctx, obj);
    HPy_Close(ctx, obj); /* we have 'iterator' */
    while (true) {
        HPy item = HPy_IterNext(ctx, iterator);
        if (HPy_IsError(item)) goto oops;
        if (HPy_IsIterStop(item)) break;
        /* process 'item' */
        HPy_Close(ctx, item);
    }
    HPy_Close(ctx, iterator);
}
// END: foo

// BEGIN: bar
void iterate_long()
{
    /* This is allowed to fail and you should be ready to handle the fallback. */
    HPySequence_long seq = HPy_AsSequence_long(ctx, obj);
    if (HPy_Sequence_IsError_long(seq))
        goto not_a_long_sequence;
    HPy_Close(ctx, obj);
    HPy_ssize_t len = HPy_Sequence_Len_long(ctx, seq);
    for (int i=0; i<len; i++) {
        long item = HPy_Sequence_GetItem_long(ctx, seq, i);
        /* process 'item' */
    }
    HPySequenceClose_long(ctx, seq);

not_a_long_sequence:
    /* ... */
}
// END: bar
