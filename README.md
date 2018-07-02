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

    ```bash
    git clone https://github.com/xiongyihui/python-webrtc-audio-processing.git
    cd python-webrtc-audio-processing
    git submodule init && git submodule update
    python setup.py build
    sudo python setup.py install
    ```

2. using Makefile

    ```bash
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

## Usage
```python
from webrtc_audio_processing import AudioProcessingModule as AP

ap = AP(enable_vad=True, enable_ns=True)
ap.set_stream_format(16000, 1)      # set sample rate and channels
ap.set_ns_level(1)                  # NS level from 0 to 3
ap.set_vad_level(1)                 # VAD level from 0 to 3

audio_10ms = '\0' * 160 * 2         # 10ms, 16000 sample rate, 16 bits, 1 channel

# only support processing 10ms audio data each time
audio_out = ap.process_stream(audio_10ms)
print('voice: {}'.format(ap.has_voice()))
```
