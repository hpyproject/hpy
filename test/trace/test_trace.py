import pytest
from hpy.trace import get_call_counts, get_durations, set_trace_functions

@pytest.fixture
def hpy_abi():
    return "trace"

def _relative_call_count(name, call_count_0, diff):
    return get_call_counts()[name] == call_count_0[name] + diff


def _assert_unchanged_except(expected, actual, *ignore):
    import pytest
    for x in expected.keys():
        if x not in ignore and actual[x] != expected[x]:
            pytest.fail("Call count of %s unexpectedly changed (expected %d but was %d)." %
                        (x, expected[x], get_call_counts()[x]))


def test_get_call_counts(compiler):
    import pytest
    mod = compiler.make_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
        {
            if (nargs != 2) {
                HPyErr_SetString(ctx, ctx->h_TypeError, "expected exactly two args");
                return HPy_NULL;
            }
            return HPy_Add(ctx, args[0], args[1]);
        }

        @EXPORT(f)
        @INIT
    """)
    # Don't rely on absolute call count numbers since the module setup could
    # already do some API calls we don't expect.
    call_counts_0 = get_call_counts().copy()

    assert mod.f(1, 2) == 3
    assert _relative_call_count("ctx_Add", call_counts_0, 1)
    _assert_unchanged_except(call_counts_0, get_call_counts(), "ctx_Add")
    assert mod.f(10, 20) == 30
    assert _relative_call_count("ctx_Add", call_counts_0, 2)
    _assert_unchanged_except(call_counts_0, get_call_counts(), "ctx_Add")
    with pytest.raises(TypeError):
        mod.f(1, 2, 3)
    assert _relative_call_count("ctx_Add", call_counts_0, 2)
    _assert_unchanged_except(call_counts_0, get_call_counts(),
                             "ctx_Add", "ctx_Err_SetString")
    assert _relative_call_count("ctx_Err_SetString", call_counts_0, 1)
    _assert_unchanged_except(call_counts_0, get_call_counts(),
                             "ctx_Add", "ctx_Err_SetString")


def test_get_durations(compiler):
    import time
    mod = compiler.make_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            if (!HPyCallable_Check(ctx, arg)) {
                HPyErr_SetString(ctx, ctx->h_TypeError, "expected callable");
                return HPy_NULL;
            }
            return HPy_CallTupleDict(ctx, arg, HPy_NULL, HPy_NULL);
        }

        @EXPORT(f)
        @INIT
    """)
    # Don't rely on absolute durations numbers since the module setup could
    # already do some API calls we don't expect.
    durations0 = get_durations().copy()

    def callback():
        time.sleep(0.3)
        return 123

    assert mod.f(callback) == 123
    assert get_durations()["ctx_CallTupleDict"] >= durations0["ctx_CallTupleDict"] + 3e5
    _assert_unchanged_except(durations0, get_durations(), "ctx_CallTupleDict", "ctx_Callable_Check")


def test_trace_funcs(compiler):
    import pytest
    mod = compiler.make_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
        {
            if (nargs != 2) {
                HPyErr_SetString(ctx, ctx->h_TypeError, "expected exactly two args");
                return HPy_NULL;
            }
            return HPy_Add(ctx, args[0], args[1]);
        }

        @EXPORT(f)
        @INIT
    """)
    on_enter_cnt = 0
    on_exit_cnt = 0

    def on_enter(name):
        assert name == "ctx_Add"
        nonlocal on_enter_cnt
        on_enter_cnt += 1

    def on_exit(name):
        assert name == "ctx_Add"
        nonlocal on_exit_cnt
        on_exit_cnt += 1

    set_trace_functions(on_enter)
    assert on_enter_cnt == 0
    assert on_exit_cnt == 0
    assert mod.f(1, 2) == 3
    assert on_enter_cnt == 1
    assert on_exit_cnt == 0
    # on_enter should stay untouched
    set_trace_functions(on_exit=on_exit)
    assert mod.f(2, 3) == 5
    assert on_enter_cnt == 2
    assert on_exit_cnt == 1
    # clear on_enter func
    set_trace_functions(on_enter=None)
    assert mod.f(2, 3) == 5
    assert on_enter_cnt == 2
    assert on_exit_cnt == 2

    with pytest.raises(TypeError):
        set_trace_functions(1)
