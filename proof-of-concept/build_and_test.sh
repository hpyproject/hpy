set -e
ROOT=`pwd` # we expect this script to be run from the repo root


install_hpy() {
    # at the moment this install hpy.devel and hpy.universal. Eventually, we
    # will want to split those into two separate packages
    PYTHON="$1"
    pushd ${ROOT}
    ${PYTHON} -m pip install .
    popd
}

install_pof_wheel() {
    # build and install a wheel using the default options
    echo "=== build_and_install_wheel ==="
    echo
    # we use this venv just to build the wheel, and then we install the wheel
    # in the "main" python env
    echo "Create a venv to build the wheel"
    python -m venv venv/wheel_builder
    PY_BUILDER="`pwd`/venv/wheel_builder/bin/python3"
    echo
    echo "Installing hpy and requirements"
    ${PY_BUILDER} -m pip install wheel
    install_hpy ${PY_BUILDER}
    pushd proof-of-concept
    ${PY_BUILDER} -m pip install -r requirements.txt
    echo
    echo "Building wheel"
    ${PY_BUILDER} setup.py bdist_wheel
    popd
    #
    WHEEL=`ls proof-of-concept/dist/*.whl`
    echo "Created $WHEEL"
    echo
    echo "Installing $WHEEL"
    python -m pip install $WHEEL
}

test_pof() {
    # this assumes that pof is already installed, e.g. after calling
    # install_pof_wheel
    python -m pip install pytest pytest-azurepipelines
    cd proof-of-concept
    python -m pytest
}

# ======== main code =======

# call the function mentioned as the first arg
COMMAND="$1"
shift
$COMMAND "$@"
