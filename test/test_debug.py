import pytest
from .support import HPyTest


class TestDebug(HPyTest):

    # these tests are run only with hpy_abi=='debug'. We will probably need to
    # tweak the approach to make it working with PyPy's apptests
    @pytest.fixture(params=['debug'])
    def hpy_abi(self, request):
        return request.param

    def test_get_open_handles(self):
        from hpy.universal import _debug
        mod = self.make_module("""
            HPyDef_METH(leak, "leak", leak_impl, HPyFunc_O)
            static HPy leak_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy_Dup(ctx, arg); // leak!
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(leak)
            @INIT
        """)
        gen1 = _debug.new_generation()
        mod.leak('hello')
        mod.leak('world')
        gen2 = _debug.new_generation()
        mod.leak('a younger leak')
        leaks1 = _debug.get_open_handles(gen1)
        leaks2 = _debug.get_open_handles(gen2)
        leaks1 = [dh.obj for dh in leaks1]
        leaks2 = [dh.obj for dh in leaks2]
        assert leaks1 == ['a younger leak', 'world', 'hello']
        assert leaks2 == ['a younger leak']
