from .support import HPyTest, DefaultExtensionTemplate
from .test_hpytype import PointTemplate

class TestSlots(HPyTest):

    ExtensionTemplate = PointTemplate

    def test_tp_init(self):
        mod = self.make_module("""
            @DEFINE_PointObject
            @DEFINE_Point_xy
            HPyDef_SLOT(Point_new, HPy_tp_new, HPyType_GenericNew)

            HPyDef_SLOT(Point_init, HPy_tp_init, Point_init_impl)
            static int Point_init_impl(HPyContext ctx, HPy self, HPy *args,
                                       HPy_ssize_t nargs, HPy kw)
            {
                long x, y;
                if (!HPyArg_Parse(ctx, args, nargs, "ll", &x, &y))
                    return -1;

                PointObject *p = HPy_CAST(ctx, PointObject, self);
                p->x = x;
                p->y = y;
                return 0;
            }

            @EXPORT_POINT_TYPE(&Point_new, &Point_init, &Point_x, &Point_y)
            @INIT
        """)
        p = mod.Point(1, 2)
        assert p.x == 1
        assert p.y == 2

    def test_sq_item(self):
        mod = self.make_module("""
            @DEFINE_PointObject

            HPyDef_SLOT(Point_getitem, HPy_sq_item, Point_getitem_impl);
            static HPy Point_getitem_impl(HPyContext ctx, HPy self, HPy_ssize_t idx)
            {
                return HPyLong_FromLong(ctx, (long)idx*2);
            }

            @EXPORT_POINT_TYPE(&Point_getitem)
            @INIT
        """)
        p = mod.Point()
        assert p[4] == 8
        assert p[21] == 42

    def test_tp_destroy(self):
        import gc
        mod = self.make_module("""
            @DEFINE_PointObject
            @DEFINE_Point_new

            static long destroyed_x;

            HPyDef_SLOT(Point_destroy, HPy_tp_destroy, Point_destroy_impl)
            static void Point_destroy_impl(void *obj)
            {
                PointObject *point = (PointObject *)obj;
                destroyed_x += point->x;
            }

            HPyDef_METH(get_destroyed_x, "get_destroyed_x", get_destroyed_x_impl, HPyFunc_NOARGS)
            static HPy get_destroyed_x_impl(HPyContext ctx, HPy self)
            {
                return HPyLong_FromLong(ctx, destroyed_x);
            }

            @EXPORT_POINT_TYPE(&Point_new, &Point_destroy)
            @EXPORT(get_destroyed_x)
            @INIT
        """)
        point = mod.Point(7, 3)
        assert mod.get_destroyed_x() == 0
        del point
        gc.collect()
        assert mod.get_destroyed_x() == 7
        gc.collect()
        assert mod.get_destroyed_x() == 7
