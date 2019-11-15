struct _HPyContext_s global_ctx = {
    .version = 1,
    .module_Create = &Module_Create,
    .none_Get = &None_Get,
    .callRealFunctionFromTrampoline = &CallRealFunctionFromTrampoline,
    .fromPyObject = &FromPyObject,
    .asPyObject = &AsPyObject,
    .dup = &Dup,
    .close = &Close,
    .long_FromLong = &Long_FromLong,
    .arg_VaParse = &Arg_VaParse,
    .number_Add = &Number_Add,
    .unicode_FromString = &Unicode_FromString,
};
