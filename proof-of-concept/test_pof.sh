set -e
ROOT=`pwd` # we expect this script to be run from the repo root

_install_hpy() {
    echo "Installing hpy"
    # at the moment this install hpy.devel and hpy.universal. Eventually, we
    # will want to split those into two separate packages
    PYTHON="$1"
    pushd ${ROOT}
    ${PYTHON} -m pip install wheel
    ${PYTHON} -m pip install .
    popd
}

_test_pof() {
    echo "==== testing pof ===="
    # this assumes that pof is already installed, e.g. after calling
    # install_pof_wheel
    python3 -m pip install pytest pytest-azurepipelines
    cd proof-of-concept
    python3 -m pytest
}

_build_wheel() {
    # we use this venv just to build the wheel, and then we install the wheel
    # in the currently active virtualenv
    echo "Create venv: wheel_builder"
    python3 -m venv venv/wheel_builder
    PY_BUILDER="`pwd`/venv/wheel_builder/bin/python3"
    echo
    echo "Installing hpy and requirements"
    _install_hpy ${PY_BUILDER}
    pushd proof-of-concept
    ${PY_BUILDER} -m pip install -r requirements.txt
    echo
    echo "Building wheel"
    ${PY_BUILDER} setup.py bdist_wheel
    popd
}

_myrm() {
    if [ -d "$1" ]
    then
        echo "rm $1"
        rm -rf "$1"
    else
        echo "skipping $1"
    fi
}

clean() {
    echo "=== cleaning up old stuff ==="
    _myrm ${ROOT}/venv/wheel_builder
    _myrm ${ROOT}/venv/wheel_runner
    _myrm ${ROOT}/venv/cpython_abi
    _myrm ${ROOT}/venv/universal_abi
    _myrm ${ROOT}/build
    _myrm ${ROOT}/proof-of-concept/build
    _myrm ${ROOT}/proof-of-concept/dist
    echo
}

wheel() {
    # build a wheel, install and test
    clean
    echo "=== testing setup.py bdist_wheel ==="
    _build_wheel
    WHEEL=`ls proof-of-concept/dist/*.whl`
    echo "Wheel created: ${WHEEL}"
    echo
    echo "Create venv: wheel_runner"
    python3 -m venv venv/wheel_runner
    source venv/wheel_runner/bin/activate
    echo "Installing wheel"
    python3 -m pip install $WHEEL
    echo
    _test_pof
}

setup_py_cpython() {
    # install using setup.py, CPython ABI
    clean
    echo "=== testing setup.py --hpy-abi=cpython install ==="
    echo "Create venv: cpython_abi"
    python3 -m venv venv/cpython_abi
    source venv/cpython_abi/bin/activate
    _install_hpy python
    echo
    echo "Running setup.py"
    pushd proof-of-concept
    python3 setup.py --hpy-abi=cpython install
    popd
    echo
    _test_pof
}

build_ext_inplace_universal() {
    # install using setup.py build_ext -i, Universal ABI
    clean
    echo "=== testing setup.py --hpy-abi=universal build_ext -i ==="
    echo "Create venv: universal_abi"
    python3 -m venv venv/universal_abi
    source venv/universal_abi/bin/activate
    _install_hpy python
    echo
    echo "Running setup.py"
    pushd proof-of-concept
    python3 setup.py --hpy-abi=universal build_ext --inplace
    popd
    echo
    export HPY_UNIVERSAL=1
    _test_pof
}


# ======== main code =======

# call the function mentioned as the first arg
COMMAND="$1"
shift
$COMMAND "$@"
