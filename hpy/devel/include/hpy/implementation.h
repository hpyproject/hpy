#include "hpy/autogen_impl.h"

HPyAPI_STORAGE int
_HPy_IMPL_NAME(Err_Occurred)(HPyContext *ctx) {
    return PyErr_Occurred() ? 1 : 0;
}
