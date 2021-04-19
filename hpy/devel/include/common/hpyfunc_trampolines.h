#ifndef HPY_COMMON_HPYFUNC_TRAMPOLINES_H
#define HPY_COMMON_HPYFUNC_TRAMPOLINES_H

#include "autogen_hpyfunc_declare.h"

#ifdef HPY_UNIVERSAL_ABI
#  include "universal/hpyfunc_trampolines.h"
#  include "universal/autogen_hpyfunc_trampolines.h"
#else
#  include "cpython/hpyfunc_trampolines.h"
#  include "cpython/autogen_hpyfunc_trampolines.h"
#endif // HPY_UNIVERSAL_ABI

#endif /* HPY_COMMON_HPYFUNC_TRAMPOLINES_H */
