"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestObject(HPyTest):
    def test_getattr(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy name, result;
                name = HPyUnicode_FromString(ctx, "foo");
                if (HPy_IsNull(name))
                    return HPy_NULL;
                result = HPy_GetAttr(ctx, arg, name);
                HPy_Close(ctx, name);
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            def __init__(self, **kw):
                for k, v in kw.items():
                    setattr(self, k, v)

        class ClassAttr:
            foo = 10

        class PropAttr:
            @property
            def foo(self):
                return 11

        assert mod.f(Attrs(foo=5)) == 5
        with pytest.raises(AttributeError):
            mod.f(Attrs())
        with pytest.raises(AttributeError):
            mod.f(42)
        assert mod.f(ClassAttr) == 10
        assert mod.f(ClassAttr()) == 10
        assert mod.f(PropAttr()) == 11

    def test_getattr_s(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy result;
                result = HPy_GetAttr_s(ctx, arg, "foo");
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            def __init__(self, **kw):
                for k, v in kw.items():
                    setattr(self, k, v)

        class ClassAttr:
            foo = 10

        class PropAttr:
            @property
            def foo(self):
                return 11

        assert mod.f(Attrs(foo=5)) == 5
        with pytest.raises(AttributeError):
            mod.f(Attrs())
        with pytest.raises(AttributeError):
            mod.f(42)
        assert mod.f(ClassAttr) == 10
        assert mod.f(ClassAttr()) == 10
        assert mod.f(PropAttr()) == 11

    def test_hasattr(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy name;
                int result;
                name = HPyUnicode_FromString(ctx, "foo");
                if (HPy_IsNull(name))
                    return HPy_NULL;
                result = HPy_HasAttr(ctx, arg, name);
                HPy_Close(ctx, name);
                if (result == -1)
                    return HPy_NULL;
                if (result)
                    return HPy_Dup(ctx, ctx->h_True);
                return HPy_Dup(ctx, ctx->h_False);
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            def __init__(self, **kw):
                for k, v in kw.items():
                    setattr(self, k, v)

        class ClassAttr:
            foo = 10

        class PropAttr:
            @property
            def foo(self):
                return 11

        class PropAttrRaising:
            @property
            def foo(self):
                raise RuntimeError


        assert mod.f(Attrs(foo=5)) is True
        assert mod.f(Attrs()) is False
        assert mod.f(42) is False
        assert mod.f(ClassAttr) is True
        assert mod.f(ClassAttr()) is True
        assert mod.f(PropAttr()) is True
        assert mod.f(PropAttrRaising()) is False


    def test_hasattr_s(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_HasAttr_s(ctx, arg, "foo");
                if (result == -1)
                    return HPy_NULL;
                if (result)
                    return HPy_Dup(ctx, ctx->h_True);
                return HPy_Dup(ctx, ctx->h_False);
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            def __init__(self, **kw):
                for k, v in kw.items():
                    setattr(self, k, v)

        class ClassAttr:
            foo = 10

        class PropAttr:
            @property
            def foo(self):
                return 11

        class PropAttrRaising:
            @property
            def foo(self):
                raise RuntimeError

        assert mod.f(Attrs(foo=5)) is True
        assert mod.f(Attrs()) is False
        assert mod.f(42) is False
        assert mod.f(ClassAttr) is True
        assert mod.f(ClassAttr()) is True
        assert mod.f(PropAttr()) is True
        assert mod.f(PropAttrRaising()) is False

    def test_setattr(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy name;
                int result;
                name = HPyUnicode_FromString(ctx, "foo");
                if (HPy_IsNull(name))
                    return HPy_NULL;
                result = HPy_SetAttr(ctx, arg, name, ctx->h_True);
                HPy_Close(ctx, name);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            pass

        class ClassAttr:
            pass

        class ReadOnlyPropAttr:
            @property
            def foo(self):
                return 11

        class WritablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

        a = Attrs()
        mod.f(a)
        assert a.foo is True

        mod.f(ClassAttr)
        assert ClassAttr.foo is True
        assert ClassAttr().foo is True

        with pytest.raises(AttributeError):
            mod.f(object())

        with pytest.raises(AttributeError):
            mod.f(ReadOnlyPropAttr())

        b = WritablePropAttr()
        mod.f(b)
        assert b.foo is True

    def test_setattr_s(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_SetAttr_s(ctx, arg, "foo", ctx->h_True);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)

        class Attrs:
            pass

        class ClassAttr:
            pass

        class ReadOnlyPropAttr:
            @property
            def foo(self):
                return 11

        class WritablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

        a = Attrs()
        mod.f(a)
        assert a.foo is True

        mod.f(ClassAttr)
        assert ClassAttr.foo is True
        assert ClassAttr().foo is True

        with pytest.raises(AttributeError):
            mod.f(object())

        with pytest.raises(AttributeError):
            mod.f(ReadOnlyPropAttr())

        b = WritablePropAttr()
        mod.f(b)
        assert b.foo is True

    def test_delattr(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(del_foo, "del_foo", HPyFunc_O)
            static HPy del_foo_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy name;
                int result;
                name = HPyUnicode_FromString(ctx, "foo");
                if (HPy_IsNull(name))
                    return HPy_NULL;
                result = HPy_DelAttr(ctx, arg, name);
                HPy_Close(ctx, name);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(del_foo)
            @INIT
        """)

        class Attrs:
            pass

        class ClassAttr:
            pass

        class WritablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

        class DeletablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

            @foo.deleter
            def foo(self):
                del self._foo

        def set_foo(obj):
            obj.foo = True

        a = Attrs()
        set_foo(a)
        assert a.foo is True
        mod.del_foo(a)
        with pytest.raises(AttributeError):
            a.foo

        set_foo(ClassAttr)
        assert ClassAttr.foo is True
        assert ClassAttr().foo is True
        mod.del_foo(ClassAttr)
        with pytest.raises(AttributeError):
            ClassAttr.foo
        with pytest.raises(AttributeError):
            ClassAttr().foo

        b = WritablePropAttr()
        set_foo(b)
        assert b.foo is True
        with pytest.raises(AttributeError):
            # does not provide a delete function, so it fails
            mod.del_foo(b)

        c = DeletablePropAttr()
        set_foo(c)
        assert c.foo is True
        mod.del_foo(c)
        with pytest.raises(AttributeError):
            c.foo

    def test_delattr_s(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(del_foo, "del_foo", HPyFunc_O)
            static HPy del_foo_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_DelAttr_s(ctx, arg, "foo");
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(del_foo)
            @INIT
        """)

        class Attrs:
            pass

        class ClassAttr:
            pass

        class WritablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

        class DeletablePropAttr:
            @property
            def foo(self):
                return self._foo

            @foo.setter
            def foo(self, value):
                self._foo = value

            @foo.deleter
            def foo(self):
                del self._foo

        def set_foo(obj):
            obj.foo = True

        a = Attrs()
        set_foo(a)
        assert a.foo is True
        mod.del_foo(a)
        with pytest.raises(AttributeError):
            a.foo

        set_foo(ClassAttr)
        assert ClassAttr.foo is True
        assert ClassAttr().foo is True
        mod.del_foo(ClassAttr)
        with pytest.raises(AttributeError):
            ClassAttr.foo
        with pytest.raises(AttributeError):
            ClassAttr().foo

        b = WritablePropAttr()
        set_foo(b)
        assert b.foo is True
        with pytest.raises(AttributeError):
            # does not provide a delete function, so it fails
            mod.del_foo(b)

        c = DeletablePropAttr()
        set_foo(c)
        assert c.foo is True
        mod.del_foo(c)
        with pytest.raises(AttributeError):
            c.foo

    def test_getitem(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy key, result;
                key = HPyLong_FromLong(ctx, 3);
                if (HPy_IsNull(key))
                    return HPy_NULL;
                result = HPy_GetItem(ctx, arg, key);
                HPy_Close(ctx, key);
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({3: "hello"}) == "hello"
        with pytest.raises(KeyError) as exc:
            mod.f({1: "bad"})
        assert exc.value.args == (3,)

        assert mod.f([0, 1, 2, "hello"]) == "hello"
        with pytest.raises(IndexError):
            mod.f([])

    def test_getitem_i(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy result;
                result = HPy_GetItem_i(ctx, arg, 3);
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({3: "hello"}) == "hello"
        with pytest.raises(KeyError) as exc:
            mod.f({1: "bad"})
        assert exc.value.args == (3,)

        assert mod.f([0, 1, 2, "hello"]) == "hello"
        with pytest.raises(IndexError):
            mod.f([])

    def test_getitem_s(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy result;
                result = HPy_GetItem_s(ctx, arg, "limes");
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({"limes": "hello"}) == "hello"
        with pytest.raises(KeyError) as exc:
            mod.f({"oranges": "bad"})
        assert exc.value.args == ("limes",)

        with pytest.raises(TypeError):
            mod.f([])

    def test_getslice(self):
        import pytest
        if self.is_graalpy:
            pytest.skip("Not yet implemented on GraalPy")
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                HPy seq;
                HPy_ssize_t i1, i2;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "Onn", &seq, &i1, &i2)) {
                    return HPy_NULL;
                }
                if (HPy_Is(ctx, seq, ctx->h_None)) {
                    seq = HPy_NULL;
                }
                return HPy_GetSlice(ctx, seq, i1, i2);
            }
            @EXPORT(f)
            @INIT
        """)
        l = [1,2,3,4,5]
        assert mod.f(l, 0, 5) == l
        assert mod.f(l, 2, 3) == [3]
        assert mod.f(l, 1, 3) == [2, 3]

        s = "hello"
        assert mod.f(s, 0, 5) == "hello"
        assert mod.f(s, 1, 3) == "el"

        class Sliceable:
            def __getitem__(self, key):
                # assume 'key' is a slice
                if key.start < 0:
                    raise ValueError
                return key.start + key.stop

        o = Sliceable()
        assert mod.f(o, 5, 13) == 18

        # test that errors are propagated
        with pytest.raises(ValueError):
            mod.f(o, -1, 1)

        # 'None' will be mapped to 'HPy_NULL' by the C function
        with pytest.raises(SystemError):
            mod.f(None, 0, 1)
        with pytest.raises(TypeError):
            mod.f(123, 0, 1)

    def test_setitem(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy key;
                int result;
                key = HPyLong_FromLong(ctx, 3);
                if (HPy_IsNull(key))
                    return HPy_NULL;
                result = HPy_SetItem(ctx, arg, key, ctx->h_True);
                HPy_Close(ctx, key);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({}) == {3: True}
        assert mod.f({"a": 1}) == {"a": 1, 3: True}
        assert mod.f({3: False}) == {3: True}

        assert mod.f([0, 1, 2, False]) == [0, 1, 2, True]
        with pytest.raises(IndexError):
            mod.f([])

    def test_setitem_i(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_SetItem_i(ctx, arg, 3, ctx->h_True);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({}) == {3: True}
        assert mod.f({"a": 1}) == {"a": 1, 3: True}
        assert mod.f({3: False}) == {3: True}

        assert mod.f([0, 1, 2, False]) == [0, 1, 2, True]
        with pytest.raises(IndexError):
            mod.f([])

    def test_setitem_s(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_SetItem_s(ctx, arg, "limes", ctx->h_True);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f({}) == {"limes": True}
        assert mod.f({"a": 1}) == {"a": 1, "limes": True}
        assert mod.f({"limes": False}) == {"limes": True}

        with pytest.raises(TypeError):
            mod.f([])

    def test_setslice(self):
        import pytest
        if self.is_graalpy:
            pytest.skip("Not yet implemented on GraalPy")
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                int res;
                HPy seq, value;
                HPy_ssize_t i1, i2;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "OnnO", &seq, &i1, &i2, &value)) {
                    return HPy_NULL;
                }
                if (HPy_Is(ctx, seq, ctx->h_None)) {
                    seq = HPy_NULL;
                }
                res = HPy_SetSlice(ctx, seq, i1, i2, value);
                if (res == 0) {
                    return HPy_Dup(ctx, seq);
                } else if (res == -1) {
                    return HPy_NULL;
                }
                HPyErr_SetString(ctx, ctx->h_SystemError,
                    "HPy_SetSlice returned an invalid result code");
                return HPy_NULL;
            }
            @EXPORT(f)
            @INIT
        """)
        l = [1,2,3,4,5]
        val = [11,22,33,44,55]
        x = mod.f(l, 0, 5, val)
        assert x is l
        assert x is not val
        assert l == val

        l = [1,2,3,4,5]
        assert mod.f(l, 0, 5, []) == []

        l = [1,2,3,4,5]
        assert mod.f(l, 1, 3, [10, 11, 12]) == [1, 10, 11, 12, 4, 5]

        class Sliceable:
            def __setitem__(self, key, value):
                # assume 'key' is a slice
                if key.start < 0:
                    raise ValueError
                self.value = (key.start, key.stop, value)

        o = Sliceable()
        assert mod.f(o, 5, 13, [7, 8, 9]).value == (5, 13, [7, 8, 9])

        # test that errors are propagated
        with pytest.raises(ValueError):
            mod.f(o, -1, 1, [])

        # 'None' will be mapped to 'HPy_NULL' by the C function
        with pytest.raises(SystemError):
            mod.f(None, 0, 1, [])
        with pytest.raises(TypeError):
            mod.f(123, 0, 1, [])

    def test_delitem(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(delitem3, "delitem3", HPyFunc_O)
            static HPy delitem3_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy key;
                int result;
                key = HPyLong_FromLong(ctx, 3);
                if (HPy_IsNull(key))
                    return HPy_NULL;
                result = HPy_DelItem(ctx, arg, key);
                HPy_Close(ctx, key);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            
            HPyDef_METH(delitem_i3, "delitem_i3", HPyFunc_O)
            static HPy delitem_i3_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_DelItem_i(ctx, arg, 3);
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            
            HPyDef_METH(delitem_s3, "delitem_s3", HPyFunc_O)
            static HPy delitem_s3_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                int result;
                result = HPy_DelItem_s(ctx, arg, "3");
                if (result < 0)
                    return HPy_NULL;
                return HPy_Dup(ctx, arg);
            }
            
            @EXPORT(delitem3)
            @EXPORT(delitem_i3)
            @EXPORT(delitem_s3)
            @INIT
        """)
        # HPy_DelItem
        assert mod.delitem3({3: False, 4: True}) == {4: True}
        with pytest.raises(KeyError):
            mod.delitem3({})
        with pytest.raises(TypeError):
            mod.delitem3((1, 2, 3, 4))
        assert mod.delitem3([0, 1, 2, False]) == [0, 1, 2]
        with pytest.raises(IndexError):
            mod.delitem3([])

        # HPy_DelItem_i
        assert mod.delitem_i3({3: False, 4: True}) == {4: True}
        with pytest.raises(KeyError):
            mod.delitem_i3({})
        with pytest.raises(TypeError):
            mod.delitem_i3((1, 2, 3, 4))
        assert mod.delitem_i3([0, 1, 2, False]) == [0, 1, 2]
        with pytest.raises(IndexError):
            mod.delitem_i3([])

        # HPy_DelItem_s
        assert mod.delitem_s3({'3': False, '4': True}) == {'4': True}
        with pytest.raises(KeyError):
            mod.delitem_s3({})
        with pytest.raises(TypeError):
            mod.delitem_s3((1, 2, 3, 4))
        with pytest.raises(TypeError):
            mod.delitem_s3([1, 2, 3, 4])

    def test_delslice(self):
        import pytest
        if self.is_graalpy:
            pytest.skip("Not yet implemented on GraalPy")
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                int res;
                HPy seq;
                HPy_ssize_t i1, i2;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "Onn", &seq, &i1, &i2)) {
                    return HPy_NULL;
                }
                if (HPy_Is(ctx, seq, ctx->h_None)) {
                    seq = HPy_NULL;
                }
                res = HPy_DelSlice(ctx, seq, i1, i2);
                if (res == 0) {
                    return HPy_Dup(ctx, seq);
                } else if (res == -1) {
                    return HPy_NULL;
                }
                HPyErr_SetString(ctx, ctx->h_SystemError,
                    "HPy_DelSlice returned an invalid result code");
                return HPy_NULL;
            }
            @EXPORT(f)
            @INIT
        """)
        l = [1,2,3,4,5]
        x = mod.f(l, 0, 5)
        assert x is l
        assert l == []

        l = [1,2,3,4,5]
        assert mod.f(l, 1, 3) == [1, 4, 5]

        class Sliceable:
            def __delitem__(self, key):
                # assume 'key' is a slice
                if key.start < 0:
                    raise ValueError
                self.value = key.start + key.stop

        o = Sliceable()
        assert mod.f(o, 5, 13).value == 18

        # test that errors are propagated
        with pytest.raises(ValueError):
            mod.f(o, -1, 1)

        # 'None' will be mapped to 'HPy_NULL' by the C function
        with pytest.raises(SystemError):
            mod.f(None, 0, 1)
        with pytest.raises(TypeError):
            mod.f(123, 0, 1)

    def test_length(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy_ssize_t result;
                result = HPy_Length(ctx, arg);
                if (result < 0)
                    return HPy_NULL;
                return HPyLong_FromSsize_t(ctx, result);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f([5,6,7,8]) == 4
        assert mod.f({"a": 1}) == 1

    def test_contains(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                int result = HPy_Contains(ctx, args[0], args[1]);
                if (result == -1) {
                    return HPy_NULL;
                }
                return HPyLong_FromLong(ctx, result);
            }
            @EXPORT(f)
            @INIT
        """)

        class WithContains:
            def __contains__(self, item):
                return item == 42

        class WithIter:
            def __iter__(self):
                return [1, 2, 3].__iter__()

        class WithGetitem:
            def __getitem__(self, item):
                if item > 3:
                    raise IndexError()
                else:
                    return item

        class Dummy:
            pass

        assert mod.f([5, 6, 42, 7, 8], 42)
        assert not mod.f([5, 6, 42, 7, 8], 4)

        assert mod.f(WithContains(), 42)
        assert not mod.f(WithContains(), 1)

        assert mod.f(WithIter(), 2)
        assert not mod.f(WithIter(), 33)

        assert mod.f(WithGetitem(), 2)
        assert not mod.f(WithGetitem(), 33)

        import pytest
        with pytest.raises(TypeError):
            mod.f(Dummy(), 42)
    
    def test_getiter(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy iterator;
                iterator = HPy_GetIter(ctx, arg);
                if HPy_IsNull(iterator)
                    return HPy_NULL;
                return iterator;
            }
            @EXPORT(f)
            @INIT
        """)

        def test_for_loop(iterator):
            results = []
            for obj in iterator:
                results.append(obj)
            return results

        class WithIter:
            def __iter__(self):
                return (1, 2, 3).__iter__()

        class WithoutIter:
            pass

        case = [1, 2, 3]
        result = mod.f(case)
        assert result
        assert test_for_loop(result) == [1, 2, 3]

        case = iter([1, 2, 3])
        result = mod.f(case)
        assert result
        assert test_for_loop(result) == [1, 2, 3]

        case = zip((1, 2, 3), [4, 5, 6])
        result = mod.f(case)
        assert result
        assert test_for_loop(result) == [(1, 4), (2, 5), (3, 6)]

        case = range(10)
        result = mod.f(case)
        assert result
        assert test_for_loop(result) == [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        case = WithIter()
        result = mod.f(case)
        assert result
        assert test_for_loop(result) == [1, 2, 3]

        import pytest
        with pytest.raises(TypeError):
            assert mod.f(WithoutIter())


    def test_dump(self):
        # _HPy_Dump is supposed to be used e.g. inside a gdb session: it
        # prints various about the given handle to stdout, and it's
        # implementation-specific. As such, it's hard to write a meaningful
        # test: let's just call it an check it doesn't crash.
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                _HPy_Dump(ctx, arg);
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)
        mod.f('hello')

    def test_type(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                return HPy_Type(ctx, arg);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f('hello') is str
        assert mod.f(42) is int

    def test_typecheck(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                HPy a, b;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "OO", &a, &b))
                    return HPy_NULL;
                int res = HPy_TypeCheck(ctx, a, b);
                return HPyBool_FromLong(ctx, res);
            }
            @EXPORT(f)
            @INIT
        """)
        class MyStr(str):
            pass
        assert mod.f('hello', str)
        assert not mod.f('hello', int)
        assert mod.f(MyStr('hello'), str)

    def test_is(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, const HPy *args, size_t nargs)
            {
                HPy obj, other;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "OO", &obj, &other))
                    return HPy_NULL;
                int res = HPy_Is(ctx, obj, other);
                return HPyBool_FromLong(ctx, res);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(None, None)
        a = object()
        assert mod.f(a, a)
        assert not mod.f(a, None)
