""" HPyType slot tests on legacy types. """

from .test_hpytype_legacy import LegacyPointTemplate
from .test_slots import TestSlots as _TestSlots, TestSqSlots as _TestSqSlots


class TestLegacySlots(_TestSlots):

    ExtensionTemplate = LegacyPointTemplate


class TestLegacySqSlots(_TestSqSlots):

    ExtensionTemplate = LegacyPointTemplate
