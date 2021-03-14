from setuptools import setup, Extension
from codecs import open
import sys
import os

# is_windows = sys.platform.startswith('win')

def is_platform_mac():
    return sys.platform == 'darwin'

def is_platform_windows():
    return sys.platform == 'win32' or sys.platform == 'cygwin'

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


# Fix compatibility when compiling on Mac Mojave.
# Explanation: https://github.com/pandas-dev/pandas/issues/23424#issuecomment-446393981
# Code credit: https://github.com/pandas-dev/pandas/pull/24274/commits/256faf2011a12424e684a42c147e1ba7ac32c6fb
if is_platform_mac():
    import _osx_support
    import distutils.sysconfig
    if not 'MACOSX_DEPLOYMENT_TARGET' in os.environ:
        current_system = list(map(int, _osx_support._get_system_version().split('.')))
        python_osx_target_str = distutils.sysconfig.get_config_var('MACOSX_DEPLOYMENT_TARGET')
        python_osx_target = list(map(int, python_osx_target_str.split('.')))
        if python_osx_target < [10, 9] and current_system >= [10, 9]:
            os.environ['MACOSX_DEPLOYMENT_TARGET'] = '10.9'

    os.environ['CC'] = 'clang'
    os.environ['CXX'] = 'clang'

if is_platform_windows():
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

if is_platform_mac():
    ext_modules = [
        Extension('pycspade.cspade',
                  sourcefiles + extra_files,
                  include_dirs=['csrc/'],
                  language='c++',
                  extra_compile_args=extra_compiler_args,
                  extra_link_args=["-O2", "-march=native", '-stdlib=libc++'],
                  ),
    ]
else:
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
    version='0.6.6',
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
