import textwrap
import difflib
import py
import pytest
from hpy.tools.autogen.parse import HPyAPI
from hpy.tools.autogen.ctx import autogen_ctx_h, autogen_ctx_def_h
from hpy.tools.autogen.trampolines import autogen_trampolines_h, autogen_impl_h
from hpy.tools.autogen.hpyslot import autogen_hpyslot_h

def src_equal(exp, got):
    # try to compare two C sources, ignoring whitespace
    exp = textwrap.dedent(exp).strip()
    got = textwrap.dedent(got).strip()
    if exp.split() != got.split():
        diff = difflib.unified_diff(exp.splitlines(), got.splitlines(),
                                    fromfile='expected',
                                    tofile='got')
        print()
        for line in diff:
            print(line)
        return False
    return True

@pytest.mark.usefixtures('initargs')
class BaseTestAutogen:

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


class TestHPyAPI(BaseTestAutogen):

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

    def test_hpyslot(self):
        api = self.parse("""
            typedef enum {
                HPy_nb_add = SLOT(7, HPyFunc_BINARYFUNC),
                HPy_tp_repr = SLOT(66, HPyFunc_REPRFUNC),
            } HPySlot_Slot;
        """)
        nb_add = api.get_slot('HPy_nb_add')
        assert nb_add.value == '7'
        assert nb_add.hpyfunc == 'HPyFunc_BINARYFUNC'
        #
        tp_repr = api.get_slot('HPy_tp_repr')
        assert tp_repr.value == '66'
        assert tp_repr.hpyfunc == 'HPyFunc_REPRFUNC'


class TestAutoGen(BaseTestAutogen):

    def test_autogen_ctx_h(self):
        api = self.parse("""
            HPy h_None;
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
        """)
        got = autogen_ctx_h(api).generate()
        exp = """
            struct _HPyContext_s {
                int ctx_version;
                HPy h_None;
                HPy (*ctx_Add)(HPyContext ctx, HPy h1, HPy h2);
            };
        """
        assert src_equal(exp, got)

    def test_autogen_ctx_def_h(self):
        api = self.parse("""
            HPy h_None;
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
        """)
        got = autogen_ctx_def_h(api).generate()
        exp = """
            struct _HPyContext_s global_ctx = {
                .ctx_version = 1,
                .h_None = (HPy){CONSTANT_H_NONE},
                .ctx_Add = &ctx_Add,
            };
        """
        assert src_equal(exp, got)

    def test_autogen_trampolines_h(self):
        api = self.parse("""
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
            void HPy_Close(HPyContext ctx, HPy h);
            void* _HPy_Cast(HPyContext ctx, HPy h);
        """)
        got = autogen_trampolines_h(api).generate()
        exp = """
            static inline HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2) {
                return ctx->ctx_Add ( ctx, h1, h2 );
            }

            static inline void HPy_Close(HPyContext ctx, HPy h) {
                ctx->ctx_Close ( ctx, h );
            }

            static inline void *_HPy_Cast(HPyContext ctx, HPy h) {
                return ctx->ctx_Cast ( ctx, h );
            }
        """
        assert src_equal(got, exp)

    def test_autogen_impl_h(self):
        api = self.parse("""
            HPy HPy_Dup(HPyContext ctx, HPy h);
            HPy HPy_Add(HPyContext ctx, HPy h1, HPy h2);
            HPy HPyLong_FromLong(HPyContext ctx, long value);
            char* HPyBytes_AsString(HPyContext ctx, HPy h);
        """)
        got = autogen_impl_h(api).generate()
        exp = """
            HPyAPI_STORAGE
            HPy _HPy_IMPL_NAME_NOPREFIX(Add)(HPyContext ctx, HPy h1, HPy h2)
            {
                return _py2h(PyNumber_Add(_h2py(h1), _h2py(h2)));
            }

            HPyAPI_STORAGE
            HPy _HPy_IMPL_NAME(Long_FromLong)(HPyContext ctx, long value)
            {
                return _py2h(PyLong_FromLong(value));
            }

            HPyAPI_STORAGE
            char *_HPy_IMPL_NAME(Bytes_AsString)(HPyContext ctx, HPy h)
            {
                return PyBytes_AsString(_h2py(h));
            }
        """
        assert src_equal(got, exp)

    def test_autogen_hpyslot_h(self):
        api = self.parse("""
            typedef enum {
                HPy_nb_add = SLOT(7, HPyFunc_BINARYFUNC),
                HPy_tp_repr = SLOT(66, HPyFunc_REPRFUNC),
            } HPySlot_Slot;
        """)
        got = autogen_hpyslot_h(api).generate()
        exp = """
            typedef enum {
                HPy_nb_add = 7,
                HPy_tp_repr = 66,
            } HPySlot_Slot;

        #define _HPySlot_SIG__HPy_nb_add HPyFunc_BINARYFUNC
        #define _HPySlot_SIG__HPy_tp_repr HPyFunc_REPRFUNC
        """
        assert src_equal(got, exp)
