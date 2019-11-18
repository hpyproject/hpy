echo `pwd`
echo `ls`
python -m virtualenv venv/builder
PY_BUILDER="`pwd`/venv/builder/bin/python3"
echo "Running with $PY_BUILDER"
cd ../proof-of-concept
# ${PY_BUILDER} -m pip install -r requirements.txt
${PY_BUILDER} -m pip install ../hpy-api
${PY_BUILDER} setup.py bdist_wheel
WHEEL=`ls dist/*.whl`
echo "Created $WHEEL"
