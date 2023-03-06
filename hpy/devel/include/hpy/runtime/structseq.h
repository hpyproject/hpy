#ifndef HPY_COMMON_RUNTIME_STRUCTSEQ_H
#define HPY_COMMON_RUNTIME_STRUCTSEQ_H

#include "hpy.h"

/*
 * Struct sequences are relatively simple subclasses of tuple, so we provide the
 * simplified API to create them here. This maps closely to the CPython limited
 * API for creating struct sequences. However, in universal mode we use the
 * collections.namedtuple type to implement this, which behaves a bit
 * differently w.r.t. hidden elements. Thus, the n_in_sequence field available
 * in CPython's PyStructSequence_Desc is not available. Also, we use a builder
 * API like for tuples and lists so that the struct sequence is guaranteed not
 * to be written after it is created.
 */

/**
 * Describes a field of a struct sequence.
 */
typedef struct {
    /**
     * Name (UTF-8 encoded) for the field or ``NULL`` to end the list of named
     * fields. Set the name to :c:var:`HPyStructSequence_UnnamedField` to leave
     * it unnamed.
     */
    const char *name;

    /**
     * Docstring of the field (UTF-8 encoded); may be ``NULL``.
     */
    const char *doc;
} HPyStructSequence_Field;

/**
 * Contains the meta information of a struct sequence type to create.
 * Struct sequences are subclasses of tuple. All fields are Python objects. The
 * index in the :c:member:`fields` array of the descriptor determines which
 * field of the struct sequence is described.
 */
typedef struct {
    /**
     * Name of the struct sequence type (UTF-8 encoded; must not be ``NULL``).
     */
    const char *name;

    /** Docstring of the type (UTF-8 encoded); may be ``NULL``. */
    const char *doc;

    /**
     * Pointer to ``NULL``-terminated array with field names of the new type
     * (must not be ``NULL``).
     */
    HPyStructSequence_Field *fields;
} HPyStructSequence_Desc;

/**
 * A marker that can be used as struct sequence field name to indicate that a
 * field should be anonymous (i.e. cannot be accessed by a name but only by
 * numeric index).
 */
extern const char * const HPyStructSequence_UnnamedField;

typedef HPyTupleBuilder HPyStructSequenceBuilder;

/**
 * Use this macro to check if a struct sequence builder is the ``NULL`` builder.
 *
 * :param x:
 *     The struct sequence builder to test.
 */
#define HPyStructSequenceBuilder_IsNull(x) ((x)._tup == 0)

/**
 * Create a new struct sequence type from a descriptor. Instances of the
 * resulting type can be created with :c:func:`HPyStructSequence_New`.
 *
 * :param ctx:
 *     The execution context.
 * :param desc:
 *     The descriptor of the struct sequence type to create (must not be
 *     ``NULL``):
 *
 * :returns:
 *     A handle to the new struct sequence type or ``HPy_NULL`` in case of
 *     errors.
 */
HPyAPI_HELPER HPy
HPyStructSequence_NewType(HPyContext *ctx, HPyStructSequence_Desc *desc);

/**
 * Creates a new builder instance for instances of ``type``.
 *
 * Since struct sequences are immutable objects, they can only be created using
 * a builder. This function will create a fresh builder for creating instances
 * of the provided struct sequence type. The type must have been created with
 * :c:func:`HPyStructSequence_NewType`.
 *
 * :param ctx:
 *     The execution context.
 * :param type:
 *     A struct sequence type (must not be ``HPy_NULL``). If the passed object
 *     is not a type, the behavior is undefined. If the given type is not
 *     appropriate, a ``TypeError`` will be raised.
 *
 * :returns:
 *     A new builder for creating a new instance of ``type``. Use macro
 *     :c:macro:`HPyStructSequenceBuilder_IsNull` to test if an error occurred.
 */
HPyAPI_HELPER HPyStructSequenceBuilder
HPyStructSequenceBuilder_New(HPyContext *ctx, HPy type);

/**
 * Sets the field at index ``idx`` of the struct sequence builder.
 *
 * Use this to collect all elements that will be used to fill the new instance
 * of the struct sequence as soon as :c:func:`HPyStructSequenceBuilder_Build` is
 * called.
 *
 * :param ctx:
 *     The execution context.
 * :param builder:
 *     A struct sequence type (must not be ``HPy_NULL``). If the given type is
 *     not appropriate, a ``TypeError`` will be raised.
 * :param idx:
 *     The index of the field to set. No bounds checking will be done. If the
 *     index is out of bounds, behavior is undefined.
 * :param value:
 *     An arbitrary object to set as value at the given index (must not be
 *     ``HPy_NULL``).
 */
HPyAPI_HELPER void
HPyStructSequenceBuilder_Set(HPyContext *ctx, HPyStructSequenceBuilder builder, HPy_ssize_t idx, HPy value);

/**
 * Sets the field at index ``idx`` of the struct sequence builder.
 *
 * Use this to collect all elements that will be used to fill the new instance
 * of the struct sequence as soon as :c:func:`HPyStructSequenceBuilder_Build` is
 * called.
 *
 * :param ctx:
 *     The execution context.
 * :param builder:
 *     A struct sequence type (must not be ``HPy_NULL``). If the given type is
 *     not appropriate, a ``TypeError`` will be raised.
 * :param idx:
 *     The index of the field to set. No bounds checking will be done. If the
 *     index is out of bounds, behavior is undefined.
 * :param value:
 *     A C long to set as value at the given index. The long will be converted
 *     to a Python ``int``.
 */
HPyAPI_HELPER void
HPyStructSequenceBuilder_Set_i(HPyContext *ctx, HPyStructSequenceBuilder builder, HPy_ssize_t idx, long value);

/**
 * Build a struct sequence from a builder.
 *
 * :param ctx:
 *     The execution context.
 * :param builder:
 *     A struct sequence type (must not be ``HPy_NULL``). If the given type is
 *     not appropriate, a ``TypeError`` will be raised.
 *
 * :returns:
 *     An HPy handle to a struct sequence containing the values inserted with
 *     :c:func:`HPyStructSequenceBuilder_Set` or
 *     :c:func:`HPyStructSequenceBuilder_Set_i`. The result will be ``HPy_NULL``
 *     in case an error occurred during building or earlier when creating the
 *     builder or setting the items.
 */
HPyAPI_HELPER HPy
HPyStructSequenceBuilder_Build(HPyContext *ctx, HPyStructSequenceBuilder builder, HPy type);

/**
 * Cancel building of a struct sequence and free any acquired resources.
 *
 * This function ignores if any error occurred previously when using the tuple
 * builder.
 *
 * :param ctx:
 *     The execution context.
 * :param builder:
 *     A struct sequence type (must not be ``HPy_NULL``). If the given type is
 *     not appropriate, a ``TypeError`` will be raised.
 */
HPyAPI_HELPER void
HPyStructSequenceBuilder_Cancel(HPyContext *ctx, HPyStructSequenceBuilder builder);

#endif /* HPY_COMMON_RUNTIME_STRUCTSEQ_H */
