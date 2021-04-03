.PHONY: shared-library
shared-library: pof.c
	python setup.py build_ext -i -f

.PHONY: test
test: shared-library
	python -m pytest test_pof.py

.PHONY: universal
universal: pof.c
	python setup.py --hpy-abi=universal build_ext --inplace
