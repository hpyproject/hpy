#!/bin/bash

valgrind --tool=callgrind --instr-atstart=no python -m pytest "$@"
