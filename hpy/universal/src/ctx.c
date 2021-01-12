#include "hpy.h"
#include "handles.h"

#include "common/runtime/ctx_call.h"
#include "common/runtime/ctx_type.h"
#include "common/runtime/ctx_module.h"
#include "common/runtime/ctx_object.h"
#include "common/runtime/ctx_listbuilder.h"
#include "common/runtime/ctx_tracker.h"
#include "common/runtime/ctx_tuple.h"
#include "common/runtime/ctx_tuplebuilder.h"
#include "ctx_meth.h"
#include "ctx_misc.h"

/* expand impl functions as:
 *     static ctx_Long_FromLong(...);
 *
 * Then, they are automatically stored in the global context by
 * autogen_ctx_def
 */
#define _HPy_IMPL_NAME(name) ctx_##name
#define _HPy_IMPL_NAME_NOPREFIX(name) ctx_##name
#include "common/implementation.h"
#undef _HPy_IMPL_NAME_NOPREFIX
#undef _HPy_IMPL_NAME

#include "autogen_ctx_def.h"
