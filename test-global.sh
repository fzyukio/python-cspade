#!/usr/bin/env bash
rm pycspade/cspade.cpp pycspade/*.so;
pip uninstall -y "pycspade>=0.0.0"
python setup.py clean;
python setup.py install
python tests/example.py