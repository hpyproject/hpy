"""
In this file we check that if we use legacy features in universal mode, we
get the expected compile time errors
"""

import pytest
from .support import HPyTest, hpy_abi_only_universal

hpy_abi = hpy_abi_only_universal


class TestLegacyForbidden(HPyTest):

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
