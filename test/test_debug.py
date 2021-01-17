import pytest
from .support import HPyTest


class TestDebug(HPyTest):

    # these tests are run only with hpy_abi=='debug'. We will probably need to
    # tweak the approach to make it working with PyPy's apptests
    @pytest.fixture(params=['debug'])
    def hpy_abi(self, request):
        return request.param

    def test_open_handles(self):
        from hpy.universal import _debug
        mod = self.make_module("""
            HPyDef_METH(leak, "leak", leak_impl, HPyFunc_NOARGS)
            static HPy leak_impl(HPyContext ctx, HPy self)
            {
                HPyUnicode_FromString(ctx, "hello");
                HPyUnicode_FromString(ctx, "world");
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(leak)
            @INIT
        """)
        n = len(_debug._get_open_handles())
        mod.leak()
        leaked_handles = _debug._get_open_handles()[:-n]
        assert leaked_handles == ['world', 'hello']
