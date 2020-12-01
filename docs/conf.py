# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import os
import re

# -- Project information -----------------------------------------------------

project = "HPy"
copyright = "2019-2020, HPy Collective"
author = "HPy Collective"

# The full version, including alpha/beta/rc tags
release = "0.0.1"


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "sphinx.ext.autosectionlabel",
    "sphinx_c_autodoc",
    "sphinx_c_autodoc.viewcode",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- sphinx_c_autodoc --------------------------------------------------------

c_autodoc_roots = [
    "../hpy/devel/include/common",
    "../hpy/devel/src",
]


def pre_process(app, filename, contents, *args):
    # remove HPyAPI_RUNTIME_FUNC so that the sphinx-c-autodoc and clang
    # find and render the API functions
    contents[:] = [
        re.sub(r"HPyAPI_RUNTIME_FUNC\((.*)\)", r"\1", part)
        for part in contents
    ]


def setup(app):
    app.connect("c-autodoc-pre-process", pre_process)


if 'READTHEDOCS' in os.environ:
    # TODO: Hopefully we can remove this setting of the libclang path once
    #       readthedocs updates its docker image to Ubuntu 20.04 which
    #       supports clang-10 and clang-11.
    from clang import cindex
    cindex.Config.set_library_file(
        "/usr/lib/x86_64-linux-gnu/libclang-6.0.so.1"
    )

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]

# most the the code examples will be in C, let's make it the default
highlight_language = 'C'
