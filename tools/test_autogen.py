import py
import pytest
from autogen import AutoGen, Function, GlobalVar

@pytest.fixture(scope='module')
def autogen(tmpdir_factory):
    tmpdir = tmpdir_factory.mktemp('autogen')
    fname = tmpdir.join('foo_api.c')
    fname.write("""
        typedef int HPy;
        typedef int HPyContext;
        HPy h_None;
        HPy HPy_Dup(HPyContext ctx, HPy h);
        void HPy_Close(HPyContext ctx, HPy h);
        void* _HPy_Cast(HPyContext ctx, HPy h);
        HPy HPyNumber_Add(HPyContext ctx, HPy x, HPy y);
        HPy HPyLong_FromLong(HPyContext ctx, long value);
        char* HPyBytes_AsString(HPyContext ctx, HPy o);
    """)
    return AutoGen(str(fname))

def src_equal(a, b):
    # try to compare two C sources, ignoring whitespace
    a = a.split()
    b = b.split()
    assert a == b
    return True

class TestFunction:

    def test_ctx_decl(self, autogen):
        func = autogen.get('HPyNumber_Add')
        assert func.ctx_decl() == 'HPy (*ctx_Number_Add)(HPyContext ctx, HPy x, HPy y)'

    def test_trampoline_def(self, autogen):
        func = autogen.get('HPyNumber_Add')
        x = func.trampoline_def()
        expected = """
            static inline HPy HPyNumber_Add(HPyContext ctx, HPy x, HPy y) {
                return ctx->ctx_Number_Add ( ctx, x, y );
            }
        """
        assert src_equal(x, expected)

    def test_trampoline_def_void(self, autogen):
        func = autogen.get('HPy_Close')
        x = func.trampoline_def()
        expected = """
            static inline void HPy_Close(HPyContext ctx, HPy h) {
                ctx->ctx_Close ( ctx, h );
            }
        """
        assert src_equal(x, expected)

    def test_trampoline_def_voidstar(self, autogen):
        func = autogen.get('_HPy_Cast')
        x = func.trampoline_def()
        expected = """
            static inline void *_HPy_Cast(HPyContext ctx, HPy h) {
                return ctx->ctx_Cast ( ctx, h );
            }
        """
        assert src_equal(x, expected)

    def test_no_implementation(self, autogen):
        func = autogen.get('HPy_Dup')
        with pytest.raises(ValueError):
            func.implementation()

    def test_implementation_hpy_types(self, autogen):
        func = autogen.get('HPyNumber_Add')
        x = func.implementation()
        expected = """
            HPyAPI_STORAGE
            HPy _HPy_IMPL_NAME(Number_Add)(HPyContext ctx, HPy x, HPy y)
            {
                return _py2h(PyNumber_Add(_h2py(x), _h2py(y)));
            }
        """
        assert src_equal(x, expected)

    def test_implementation_primitive_arg(self, autogen):
        func = autogen.get('HPyLong_FromLong')
        x = func.implementation()
        expected = """
            HPyAPI_STORAGE
            HPy _HPy_IMPL_NAME(Long_FromLong)(HPyContext ctx, long value)
            {
                return _py2h(PyLong_FromLong(value));
            }
        """
        assert src_equal(x, expected)

    def test_implementation_ptr_return(self, autogen):
        func = autogen.get('HPyBytes_AsString')
        x = func.implementation()
        expected = """
            HPyAPI_STORAGE
            char *_HPy_IMPL_NAME(Bytes_AsString)(HPyContext ctx, HPy o)
            {
                return PyBytes_AsString(_h2py(o));
            }
        """
        assert src_equal(x, expected)


class TestGlobalVar:

    def test_impl_name(self, autogen):
        var = autogen.get('h_None')
        assert var.ctx_impl_name() == '(HPy){CONSTANT_H_NONE}'
