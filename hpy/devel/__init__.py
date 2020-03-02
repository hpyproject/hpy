import glob
import os

INCLUDE_DIR = os.path.join(os.path.dirname(__file__), 'include')
SOURCE_DIR = os.path.join(os.path.dirname(__file__), 'src')

def get_include():
    return INCLUDE_DIR

def get_sources():
    return glob.glob(os.path.join(SOURCE_DIR, "runtime", "*.c"))
