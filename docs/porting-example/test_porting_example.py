# -*- coding: utf-8 -*-

""" Porting example tests. """

import math


class TestPorting:
    def test_load_module(self, step):
        mod = step.import_step()
        assert type(mod.Point) == type

    def test_create_point(self, step):
        mod = step.import_step()
        p = mod.Point(1, 2)
        assert type(p) == mod.Point

    def test_norm(self, step):
        mod = step.import_step()
        assert mod.Point(1, 2).norm() == math.sqrt(5.0)

    def test_dot(self, step):
        mod = step.import_step()
        p1 = mod.Point(1, 2)
        p2 = mod.Point(3, 2)
        assert mod.dot(p1, p2) == 7.0
