#!/usr/bin/env python
# encoding: utf-8
"""
Python bindings of webrtc audio processing
"""

from glob import glob
import platform
import sys
from setuptools import setup, Extension


with open('README.md') as f:
    long_description = f.read()

include_dirs = ['src', 'webrtc-audio-processing']
libraries = ['pthread', 'stdc++']
define_macros = [
    ('WEBRTC_LINUX', None),
    ('WEBRTC_POSIX', None),
    ('WEBRTC_NS_FLOAT', None),
    ('WEBRTC_AUDIO_PROCESSING_ONLY_BUILD', None)
]
extra_compile_args = ['-std=c++11']

ap_sources = []
ap_dir_prefix = 'webrtc-audio-processing/webrtc/'
for i in range(8):
    ap_sources += glob(ap_dir_prefix + '*.c*')
    ap_dir_prefix += '*/'

# remove files for windows
ap_sources.remove('webrtc-audio-processing/webrtc/system_wrappers/source/rw_lock_generic.cc')
ap_sources.remove('webrtc-audio-processing/webrtc/system_wrappers/source/condition_variable.cc')
ap_sources = [src for src in ap_sources if src.find('_win.') < 0]

if platform.machine().find('arm') >= 0:
    ap_sources = [src for src in ap_sources if src.find('mips.') < 0 and src.find('sse') < 0]
    extra_compile_args.append('-mfloat-abi=hard')
    extra_compile_args.append('-mfpu=neon')
    define_macros.append(('WEBRTC_HAS_NEON', None))
else:
    ap_sources = [src for src in ap_sources if src.find('mips.') < 0 and src.find('neon.') < 0]


sources = (
    ap_sources +
    ['src/audio_processing_module.cpp', 'src/webrtc_audio_processing.i']
)

swig_opts = (
    ['-c++'] +
    ['-I' + h for h in include_dirs]
)


setup(
    name='webrtc_audio_processing',
    version='0.1.2',
    description='Python bindings of webrtc audio processing',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Yihui Xiong',
    author_email='yihui.xiong@hotmail.com',
    url='https://github.com/xiongyihui/python-webrtc-audio-processing',
    download_url='https://pypi.python.org/pypi/webrtc_audio_processing',
    packages=['webrtc_audio_processing'],
    ext_modules=[
        Extension(
            name='webrtc_audio_processing._webrtc_audio_processing',
            sources=sources,
            swig_opts=swig_opts,
            include_dirs=include_dirs,
            libraries=libraries,
            define_macros=define_macros,
            extra_compile_args=extra_compile_args
        )
    ],
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'License :: OSI Approved :: BSD License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: C++'
    ],
    license='BSD',
    keywords=['webrtc audioprocessing', 'voice activity detection', 'noise suppression', 'automatic gain control'],
    platforms=['Linux'],
    package_dir={
        'webrtc_audio_processing': 'src'
    }
)
