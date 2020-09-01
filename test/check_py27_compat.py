"""
Some of the files in this repo are used also by PyPy tests, which run on
python2.7.

This script tries to import all of them: it does not check any behavior, just
that they are importable and thus are not using any py3-specific syntax.
"""

from __future__ import print_function
import sys
import os.path
import glob
import traceback

ROOT = os.path.abspath(os.path.join(__file__, '..', '..'))

def try_import(name):
    try:
        print('Trying to import %s... ' % name, end='')
        __import__(name)
    except:
        print('ERROR!')
        print()
        traceback.print_exc()
        print()
        return False
    else:
        print('OK')
        return True

def try_import_hpy_devel():
    """
    To import hpy.devel we need to create an empty hpy/__init__.py, because
    python2.7 does not support namespace packages.

    Return the number of failed imports.
    """
    init_py = os.path.join(ROOT, 'hpy', '__init__.py')
    assert not os.path.exists(init_py)
    try:
        # create an empty __init__.py
        with open(init_py, 'w') as f:
            pass
        if try_import('hpy.devel'):
            return 0
        else:
            return 1 # 1 failed imports
    finally:
        os.remove(init_py)

def try_import_tests():
    failed = 0
    tests = os.path.join(ROOT, 'test', 'test_*.py')
    for fname in glob.glob(tests):
        fname = os.path.basename(fname)
        fname, _ = os.path.splitext(fname)
        if fname == 'test_support':
            continue
        if not try_import('test.%s' % fname):
            failed += 1
    return failed

def main():
    if sys.version_info[:2] != (2, 7):
        print('ERROR: this script should be run on top of python 2.7')
        sys.exit(1)

    sys.path.insert(0, ROOT)
    failed = 0
    failed += try_import_hpy_devel()
    failed += try_import_tests()
    print()
    if failed == 0:
        print('Everything ok!')
    else:
        print('%d failed imports :(' % failed)
        sys.exit(1)

if __name__ == '__main__':
    main()
