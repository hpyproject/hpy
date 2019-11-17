from pathlib import Path
from collections import defaultdict
from setuptools import setup, find_packages

def get_data_files():
    include_dir = Path('include')
    result = defaultdict(list)
    for p in include_dir.glob('**/*.h'):
        result[str(p.parent)].append(str(p))
    return list(result.items())


setup(
    name="hpy_devel",
    version="0.1",
    packages=['hpy_devel'],
    include_package_data=True
)
