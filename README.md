# WebRTC Audio Processing Module for Python

## Requirements
+ swig
+ gcc toolchain
+ python

>Note:
>If you get error when compiling with `libwebrtc-audio-processing.a`, 
>go to [wav_aec](https://github.com/xiongyihui/wav-aec) to compile your own `libwebrtc-audio-processing.a`

## Get started
```
make
python record.py output.wav
python wav_aec.py near.wav far.wav after.wav
```
