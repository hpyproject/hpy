from test.support import HPyDebugTest

class TestHandles(HPyDebugTest):

    def test_get_open_handles(self):
        from hpy.universal import _debug
        mod = self.make_leak_module()
        gen1 = _debug.new_generation()
        mod.leak('hello')
        mod.leak('world')
        gen2 = _debug.new_generation()
        mod.leak('a younger leak')
        leaks1 = _debug.get_open_handles(gen1)
        leaks2 = _debug.get_open_handles(gen2)
        leaks1 = [dh.obj for dh in leaks1]
        leaks2 = [dh.obj for dh in leaks2]
        assert leaks1 == ['hello', 'world', 'a younger leak']
        assert leaks2 == ['a younger leak']

    def test_DebugHandle_id(self):
        from hpy.universal import _debug
        mod = self.make_leak_module()
        gen = _debug.new_generation()
        mod.leak('a')
        mod.leak('b')
        a1, b1 = _debug.get_open_handles(gen)
        a2, b2 = _debug.get_open_handles(gen)
        assert a1.obj == a2.obj == 'a'
        assert b1.obj == b2.obj == 'b'
        #
        assert a1 is not a2
        assert b1 is not b2
        #
        assert a1.id == a2.id
        assert b1.id == b2.id
        assert a1.id != b1.id

    def test_DebugHandle_compare(self):
        import pytest
        from hpy.universal import _debug
        mod = self.make_leak_module()
        gen = _debug.new_generation()
        mod.leak('a')
        mod.leak('a')
        a2, a1 = _debug.get_open_handles(gen)
        assert a1 != a2 # same underlying object, but different DebugHandle
        #
        a2_new, a1_new = _debug.get_open_handles(gen)
        assert a1 is not a1_new  # different objects...
        assert a2 is not a2_new
        assert a1 == a1_new      # ...but same DebugHandle
        assert a2 == a2_new
        #
        with pytest.raises(TypeError):
            a1 < a2
        with pytest.raises(TypeError):
            a1 <= a2
        with pytest.raises(TypeError):
            a1 > a2
        with pytest.raises(TypeError):
            a1 >= a2

        assert not a1 == 'hello'
        assert a1 != 'hello'
        with pytest.raises(TypeError):
            a1 < 'hello'

    def test_DebugHandle_repr(self):
        from hpy.universal import _debug
        mod = self.make_leak_module()
        gen = _debug.new_generation()
        mod.leak('hello')
        h_hello, = _debug.get_open_handles(gen)
        assert repr(h_hello) == "<DebugHandle 0x%x for 'hello'>" % h_hello.id

    def test_LeakDetector(self):
        import pytest
        from hpy.debug import LeakDetector, HPyLeakError
        mod = self.make_leak_module()
        ld = LeakDetector()
        ld.start()
        mod.leak('hello')
        with pytest.raises(HPyLeakError) as exc:
            ld.stop()
        assert str(exc.value).startswith('1 unclosed handle:')
        #
        with pytest.raises(HPyLeakError) as exc:
            with LeakDetector():
                mod.leak('foo')
                mod.leak('bar')
                mod.leak('baz')
        msg = str(exc.value)
        assert msg.startswith('3 unclosed handles:')
        assert 'foo' in msg
        assert 'bar' in msg
        assert 'baz' in msg
        assert 'hello' not in msg
        assert 'world' not in msg

    def test_closed_handles(self):
        from hpy.universal import _debug
        mod = self.make_leak_module()
        gen = _debug.new_generation()
        mod.leak('hello')
        h_hello, = _debug.get_open_handles(gen)
        assert not h_hello.is_closed
        h_hello._force_close()
        assert h_hello.is_closed
        assert _debug.get_open_handles(gen) == []
        assert h_hello in _debug.get_closed_handles()
        assert repr(h_hello) == "<DebugHandle 0x%x CLOSED>" % h_hello.id

    def test_closed_handles_queue_max_size(self):
        from hpy.universal import _debug
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)
        old_size = _debug.get_closed_handles_queue_max_size()
        try:
            # by calling "f" we open and close 3 handles: 1 for self, 1 for arg
            # and 1 for the result. So, every call to f() increases the size of
            # closed_handles() by 3
            n1 = len(_debug.get_closed_handles())
            _debug.set_closed_handles_queue_max_size(n1+7)
            assert _debug.get_closed_handles_queue_max_size() == n1+7
            #
            mod.f('aaa')
            n2 = len(_debug.get_closed_handles())
            assert n2 == n1+3
            #
            mod.f('bbb')
            n3 = len(_debug.get_closed_handles())
            assert n3 == n2+3
            # with the next call we reach the maximum size of the queue
            mod.f('ccc')
            n4 = len(_debug.get_closed_handles())
            assert n4 == n1+7
            #
            # same as before
            mod.f('ddd')
            n5 = len(_debug.get_closed_handles())
            assert n5 == n1+7
        finally:
            _debug.set_closed_handles_queue_max_size(old_size)

    def xtest_reuse_closed_handles(self):
        from hpy.universal import _debug
        old_size = _debug.get_closed_handles_queue_max_size()
        try:
            mod = self.make_leak_module()
            gen = _debug.new_generation()
            mod.leak('h1')
            mod.leak('h2')
            h1, h2 = _debug.get_open_handles(gen)
            n = len(_debug.get_closed_handles())
            h1._force_close()
            h2._force_close()
            closed_handles = _debug.get_closed_handles()
            assert closed_handles[n:n+2] == [h1, h2]
            #
            # make sure that the closed_handles queue will be considered full
            _debug.set_closed_handles_queue_max_size(1)
            mod.leak('h3')
            mod.leak('h4')
            mod.leak('h5')
            h3, h4 = _debug.get_open_handles(gen)
            print(closed_handles.index(h3))
            print(closed_handles.index(h4))
            print(closed_handles.index(h5))
            import pdb;pdb.set_trace()

        finally:
            _debug.set_closed_handles_queue_max_size(old_size)
