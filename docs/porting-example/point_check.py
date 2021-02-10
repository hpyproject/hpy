""" Perform checks on point module porting steps. """

import pofpackage.point_capi as point_step_0
import pofpackage.point_hpy_legacy_1 as point_step_1
import pofpackage.point_hpy_legacy_2 as point_step_2
import pofpackage.point_hpy_final as point_step_3


def sanity_checks(mod):
    """ Perform sanity checks. """
    print("Sanity checks:")
    print()
    print(mod)
    print(mod.__doc__)
    print(mod.Point)
    print(mod.Point.__doc__)
    print(mod.dot)
    print(mod.dot.__doc__)
    print()


def norm_checks(mod):
    """ Perform checks on Point.norm(). """
    print("norm checks:")
    print()
    print("Point().norm()", mod.Point().norm())
    print("Point(1.5).norm()", mod.Point(1.5).norm())
    print("Point(1, 2).norm()", mod.Point(1, 2).norm())
    print()


def dot_checks(mod):
    """ Perform checks on .dot(p1, p2). """
    print("dot checks:")
    print()
    print("dot(Point(1, 2), Point(1, 2))",
          mod.dot(mod.Point(1, 2), mod.Point(1, 2)))


def all_checks(name, mod):
    """ Perform all checks on the point module. """
    print(f"{name} checks")
    print("=" * (len(name) + len(" checks")))
    print()
    sanity_checks(mod)
    norm_checks(mod)
    dot_checks(mod)
    print()


if __name__ == "__main__":
    all_checks("Step 0", point_step_0)
    all_checks("Step 1", point_step_1)
    all_checks("Step 2", point_step_2)
    all_checks("Step 3", point_step_3)
