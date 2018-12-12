from setuptools import setup, Extension
from codecs import open
import sys

is_windows = sys.platform.startswith('win')

try:
    from Cython.Distutils import build_ext
except ImportError:
    use_cython = False
else:
    use_cython = True

if use_cython:
    sourcefiles = ['pycspade/cspade.pyx']
else:
    sourcefiles = ['pycspade/cspade.cpp']

extra_files = ['csrc/{}'.format(x) for x in [
    'makebin.cc',
    'getconf.cc',
    'exttpose.cc',
    'wrappers.cc',
    'calcdb.cc',
    'TransArray.cc',
    'Array.cc',
    'Itemset.cc',
    'Lists.cc',
    'Eqclass.cc',
    'InvertDatabase.cc',
    'Partition.cc',
    'Sequence.cc',
    'common.cc',
    'argv_parser.cc',
    'SpadeArguments.cc',
    'FreqIt.cc',
    'EqGrNode.cc',
    'ClassInfo.cc'
]]

if is_windows:
    extra_compiler_args = []
else:
    extra_compiler_args = [
        '-std=c++11',
        '-Wno-sign-compare',
        '-Wno-incompatible-pointer-types',
        '-Wno-unused-variable',
        '-Wno-absolute-value',
        '-Wno-visibility',
        '-Wno-#warnings',
    ]

ext_modules = [
    Extension('pycspade.cspade',
              sourcefiles + extra_files,
              include_dirs=['csrc/'],
              language='c++',
              extra_compile_args=extra_compiler_args,
              ),
]

with open('README.md', 'r') as fh:
    long_description = fh.read()

setup_args = dict(
    name='pycspade',
    ext_modules=ext_modules,
    license='MIT',
    packages=['pycspade'],
    version='0.6.1',
    author=['Mohammed J. Zaki', 'Yukio Fukuzawa'],
    description='C-SPADE Python Implementation',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/fzyukio/python-cspade',
    keywords=['cspade', 'c-spade', 'sequence mining'],
    install_requires=['Cython'],
)

if use_cython:
    setup_args['cmdclass'] = {'build_ext': build_ext}

setup(
    **setup_args
)
