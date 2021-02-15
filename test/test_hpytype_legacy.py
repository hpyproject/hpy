""" HPyType tests on legacy types. """

from .test_hpytype import PointTemplate, TestType as _TestType


class LegacyPointTemplate(PointTemplate):
    """
    Override PointTemplate to instead define a legacy point type that
    still provides access to PyObject_HEAD.
    """

    _STRUCT_BEGIN_FORMAT = """
        #include <Python.h>
        typedef struct {{
            PyObject_HEAD
    """

    _STRUCT_END_FORMAT = """
        }} {struct_name};
        HPyType_LEGACY_HELPERS({struct_name})
    """

    _IS_LEGACY = ".legacy = true,"


class TestLegacyType(_TestType):

    ExtensionTemplate = LegacyPointTemplate
