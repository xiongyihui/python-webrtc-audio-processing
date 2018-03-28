# WebRTC Audio Processing for Python

[![Build Status](https://travis-ci.org/xiongyihui/python-webrtc-audio-processing.svg?branch=master)](https://travis-ci.org/xiongyihui/python-webrtc-audio-processing)
[![Pypi](https://img.shields.io/pypi/v/webrtc_audio_processing.svg)](https://pypi.python.org/pypi/webrtc_audio_processing)

Python binding of WebRTC Audio Processing.

## Requirements
+ swig
+ compile toolchain
+ python

## Build
There are two way to build the package.

1. using setup.py

    ```
    git clone https://github.com/xiongyihui/python-webrtc-audio-processing.git
    cd python-webrtc-audio-processing
    git submodule init && git submodule update
    python setup.py build
    sudo python setup.py install
    ```

2. using Makefile

    ```
    git clone https://github.com/xiongyihui/python-webrtc-audio-processing.git
    cd python-webrtc-audio-processing
    git submodule init && git submodule update
    cd webrtc-audio-processing
    ./autogen.sh
    ./configure --with-pic
    make
    cd ../src
    make
    ```

## Get started
```
from webrtc_audio_processing import AudioProcessingModule

```

