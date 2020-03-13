#include "hpy.h"

HPy_DEF_METH_NOARGS(do_nothing)
static HPy do_nothing_impl(HPyContext ctx, HPy self)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPy_DEF_METH_O(double_obj)
static HPy double_obj_impl(HPyContext ctx, HPy self, HPy obj)
{
    return HPyNumber_Add(ctx, obj, obj);
}

HPy_DEF_METH_VARARGS(add_ints)
static HPy add_ints_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    long a, b;
    if (!HPyArg_Parse(ctx, args, nargs, "ll", &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}

HPy_DEF_METH_KEYWORDS(add_ints_kw)
static HPy add_ints_kw_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs,
                            HPy kw)
{
    long a, b;
    const char* kwlist[] = {"a", "b", NULL};
    if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "ll", kwlist, &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}

typedef struct {
    HPyObject_HEAD
    double x;
    double y;
} HPy_Point;

HPy_DEF_METH_NOARGS(Point_repr)
static HPy Point_repr_impl(HPyContext ctx, HPy self)
{
    HPy_Point *point = HPy_CAST(ctx, HPy_Point, self);
    return HPyUnicode_FromString(ctx, "Point()");
    //return HPyUnicode_FromFormat("Point(%d, %d)", point->x, point->y);
}

static const HPyType_Slot point_type_slots[] = {
    {Py_tp_repr, Point_repr},
    {0, NULL},
};

static const HPyType_Spec point_type_spec = {
    .name = "pof.Point",
    .basicsize = sizeof(HPy_Point),
    .itemsize = 0,
    .flags = HPy_TPFLAGS_DEFAULT,
    .slots = point_type_slots,
};

static HPyMethodDef PofMethods[] = {
    {"do_nothing", do_nothing, HPy_METH_NOARGS, ""},
    {"double", double_obj, HPy_METH_O, ""},
    {"add_ints", add_ints, HPy_METH_VARARGS, ""},
    {"add_ints_kw", add_ints_kw, HPy_METH_KEYWORDS, ""},
    {NULL, NULL, 0, NULL}
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "pof",
    .m_doc = "HPy Proof of Concept",
    .m_size = -1,
    .m_methods = PofMethods
};

HPy_MODINIT(pof)
static HPy init_pof_impl(HPyContext ctx)
{
    HPy m, h_point_type;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    h_point_type = HPyType_FromSpec(ctx, &point_type_spec);
    if (HPy_IsNull(h_point_type))
      return HPy_NULL;
    HPy_SetAttr_s(ctx, m, "Point", h_point_type);
    return m;
}
