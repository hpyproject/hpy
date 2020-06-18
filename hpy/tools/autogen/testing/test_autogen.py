import py
import pytest
from hpy.tools.autogen.parse import HPyAPI
from hpy.tools.autogen.ctx import autogen_ctx_h, autogen_ctx_def_h
from hpy.tools.autogen.trampolines import autogen_trampolines_h, autogen_impl_h

def src_equal(a, b):
    # try to compare two C sources, ignoring whitespace
    a = a.split()
    b = b.split()
    assert a == b
    return True

@pytest.mark.usefixtures('initargs')
class TestAutogen:

    @pytest.fixture
    def initargs(self, tmpdir):
        self.tmpdir = tmpdir

    def parse(self, src):
        fname = self.tmpdir.join('test_api.h')
        # automatically add usefuly typedefs
        src = """
            typedef int HPy;
            typedef int HPyContext;
        """ + src
        fname.write(src)
        return HPyAPI.parse(fname)

    def autogen(self, cls, src):
        api = self.parse(src)
        return cls(api).generate()

    def test_ctx_name(self):
        api = self.parse("""
            HPy h_None;
            HPy HPy_Dup(HPyContext ctx, HPy h);
            void* _HPy_Cast(HPyContext ctx, HPy h);
        """)
        assert api.get_var('h_None').ctx_name() == 'h_None'
        assert api.get_func('HPy_Dup').ctx_name() == 'ctx_Dup'
        assert api.get_func('_HPy_Cast').ctx_name() == 'ctx_Cast'

    def test_cpython_name(self):
        api = self.parse("""
            HPy HPy_Dup(HPyContext ctx, HPy h);
            long HPyLong_AsLong(HPyContext ctx, HPy h);
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
        """)
        assert api.get_func('HPy_Dup').cpython_name is None
        assert api.get_func('HPyLong_AsLong').cpython_name == 'PyLong_AsLong'
        assert api.get_func('HPy_Add').cpython_name == 'PyNumber_Add'

    def test_autogen_ctx_h(self):
        api = self.parse("""
            HPy h_None;
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
        """)
        out = autogen_ctx_h(api).generate()
        exp = """
            struct _HPyContext_s {
                int ctx_version;
                HPy h_None;
                HPy (*ctx_Add)(HPyContext ctx, HPy h1, HPy h2);
            };
        """
        assert src_equal(out, exp)

    # WIP: the following tests are still broken and needs to be fixed

    def test_trampoline_def(self, autogen):
        func = autogen.get('HPy_Add')
        x = func.trampoline_def()
        expected = """
            static inline HPy HPy_Add(HPyContext ctx, HPy x, HPy y) {
                return ctx->ctx_Add ( ctx, x, y );
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
        func = autogen.get('HPy_Add')
        x = func.implementation()
        expected = """
            HPyAPI_STORAGE
            HPy _HPy_IMPL_NAME_NOPREFIX(Add)(HPyContext ctx, HPy x, HPy y)
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
