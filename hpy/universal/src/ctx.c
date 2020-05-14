#include "hpy.h"
#include "handles.h"

#include "ctx_module.h"
#include "ctx_meth.h"
#include "ctx_type.h"
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
