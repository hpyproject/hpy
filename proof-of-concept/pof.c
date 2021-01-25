#include "hpy.h"

HPyDef_METH(do_nothing, "do_nothing", do_nothing_impl, HPyFunc_NOARGS)
static HPy do_nothing_impl(HPyContext ctx, HPy self)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPyDef_METH(double_obj, "double", double_obj_impl, HPyFunc_O)
static HPy double_obj_impl(HPyContext ctx, HPy self, HPy obj)
{
    return HPy_Add(ctx, obj, obj);
}

HPyDef_METH(add_ints, "add_ints", add_ints_impl, HPyFunc_VARARGS)
static HPy add_ints_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    long a, b;
    if (!HPyArg_Parse(ctx, NULL, args, nargs, "ll", &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}

HPyDef_METH(add_ints_kw, "add_ints_kw", add_ints_kw_impl, HPyFunc_KEYWORDS)
static HPy add_ints_kw_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs,
                            HPy kw)
{
    long a, b;
    const char* kwlist[] = {"a", "b", NULL};
    if (!HPyArg_ParseKeywords(ctx, NULL, args, nargs, kw, "ll", kwlist, &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}

typedef struct {
    double x;
    double y;
} HPy_Point;

HPyDef_SLOT(Point_new, Point_new_impl, HPy_tp_new)
static HPy Point_new_impl (HPyContext ctx, HPy cls, HPy *args,
                           HPy_ssize_t nargs, HPy Kw)
{
    double x, y;
    if (!HPyArg_Parse(ctx, NULL, args, nargs, "dd", &x, &y))
        return HPy_NULL;
    HPy_Point *point;
    HPy h_point = HPy_New(ctx, cls, &point);
    if (HPy_IsNull(h_point))
        return HPy_NULL;
    point->x = x;
    point->y = y;
    return h_point;
}

HPyDef_SLOT(Point_repr, Point_repr_impl, HPy_tp_repr)
static HPy Point_repr_impl(HPyContext ctx, HPy self)
{
    HPy_Point *point = (HPy_Point*) HPy_Cast(ctx, self);
    return HPyUnicode_FromString(ctx, "Point(?, ?)");
    //return HPyUnicode_FromFormat("Point(%d, %d)", point->x, point->y);
}


static HPyDef *point_type_defines[] = {
    &Point_new,
    &Point_repr,
    NULL
};
static HPyType_Spec point_type_spec = {
    .name = "pof.Point",
    .basicsize = sizeof(HPy_Point),
    .flags = HPy_TPFLAGS_DEFAULT,
    .defines = point_type_defines
};

static HPyDef *module_defines[] = {
    &do_nothing,
    &double_obj,
    &add_ints,
    &add_ints_kw,
    NULL
};
static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "pof",
    .m_doc = "HPy Proof of Concept",
    .m_size = -1,
    .defines = module_defines
};

HPy_MODINIT(pof)
static HPy init_pof_impl(HPyContext ctx)
{
    HPy m, h_point_type;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    h_point_type = HPyType_FromSpec(ctx, &point_type_spec, NULL);
    if (HPy_IsNull(h_point_type))
      return HPy_NULL;
    HPy_SetAttr_s(ctx, m, "Point", h_point_type);
    return m;
}
