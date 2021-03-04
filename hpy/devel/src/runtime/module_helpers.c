/**
 * Implementation of HPyModule_AddType.
 *
 * HPyModule_AddType adds a type to a module, given an HPyType_Spec and
 * an optional list of HPyType_SpecParams.
 *
 * API
 * ---
 *
 */

#include "hpy.h"

/**
 * Create a type and add it to the given module. The type is created using
 * `HPyType_FromSpec`.
 *
 * :param ctx:
 *     The execution context.
 * :param module:
 *     A handle to the module.
 * :param name:
 *     The name of the attribute on the module to assign the type to.
 * :param hpyspec:
 *     The type spec to use to create the type.
 * :param params:
 *     The type spec parameters to use to create the type.
 *
 * :returns: 0 on failure, 1 on success.
 *
 * Examples:
 *
 * Using `HPyModule_AddType` without any `HPyType_SpecParam` parameters:
 *
 * .. code-block:: c
 *
 *     if (!HPyModule_AddType(ctx, module, "MyType", hpyspec, NULL))
 *         return HPy_NULL;
 *     ...
 *
 * Using `HPyArg_Parse` with `HPyType_SpecParam` parameters:
 *
 * .. code-block:: c
 *
 *     HPyType_SpecParam params[] = {
 *         { HPyType_SpecParam_Base, ctx->h_LongType },
 *         { 0 }
 *     };
 *
 *     if (!HPyModule_AddType(ctx, module, "MyType", hpyspec, params))
 *         return HPy_NULL;
 *     ...
 */
HPyAPI_RUNTIME_FUNC(int)
HPyModule_AddType(HPyContext ctx, HPy module, const char *name,
                  HPyType_Spec *hpyspec, HPyType_SpecParam *params)
{
    HPy h_type = HPyType_FromSpec(ctx, hpyspec, params);
    if (HPy_IsNull(h_type)) {
        return 0;
    }
    if (HPy_SetAttr_s(ctx, module, name, h_type) != 0) {
        HPy_Close(ctx, h_type);
        return 0;
    }
    HPy_Close(ctx, h_type);
    return 1;
}
