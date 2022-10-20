"""
In this file we check that if we use legacy features in universal mode, we
get the expected compile time errors
"""

import pytest
from .support import HPyTest, hpy_abi_only_universal

hpy_abi = hpy_abi_only_universal

class TestLegacyForbidden(HPyTest):

    LEGACY_ERROR = "Cannot use legacy functions when targeting the HPy Universal ABI"

    def test_expect_make_error(self):
        src = """
            #error "this is a compile time error"
        """
        self.expect_make_error(src, "this is a compile time error")

    def test_Python_h_forbidden(self, capfd):
        src = """
                #include <Python.h>
                @INIT
        """
        self.expect_make_error(src,
            "It is forbidden to #include <Python.h> "
            "when targeting the HPy Universal ABI")

    def test_HPy_AsPyObject(self, capfd):
        # NOTE: in this test we don't include Python.h. We want to test that
        # we get a nice compile-time error by just calling HPy_AsPyObject.
        # that's why we use "cpy_PyObject" (which is available because defined
        # by hpy.h)
        src = """
            HPyDef_METH(f, "f", HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                cpy_PyObject *pyobj = HPy_AsPyObject(ctx, self);
                (void)pyobj; // silence the warning about unused variable
                return HPy_NULL;
            }
            @EXPORT(f)
            @INIT
        """
        self.expect_make_error(src, self.LEGACY_ERROR)

    def test_HPy_FromPyObject(self, capfd):
        # NOTE: in this test we don't include Python.h. We want to test that
        # we get a nice compile-time error by just calling HPy_AsPyObject.
        # that's why we use "cpy_PyObject" (which is available because defined
        # by hpy.h)
        src = """
            HPyDef_METH(f, "f", HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                cpy_PyObject *pyobj = NULL;
                return HPy_FromPyObject(ctx, pyobj);
            }
            @EXPORT(f)
            @INIT
        """
        self.expect_make_error(src, self.LEGACY_ERROR)
