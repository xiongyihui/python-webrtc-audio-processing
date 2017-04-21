

SWIG := swig

WEBRTCLIBFILE = ../webrtc-audio-processing/webrtc/modules/audio_processing/.libs/libwebrtc_audio_processing.a

CXXFLAGS :=  -fPIC -std=c++11 -I. -I../webrtc-audio-processing  $(shell python-config --cflags) -DWEBRTC_AUDIO_PROCESSING_ONLY_BUILD -DWEBRTC_LINUX -DWEBRTC_POSIX -DWEBRTC_NS_FLOAT
LDFLAGS := -shared $(shell python-config --ldflags) -lpthread
CXX := g++


all: _webrtc_audio_processing.so

webrtc_audio_processing_wrap.cpp: webrtc_audio_processing.i
	$(SWIG) -I. -c++ -python -o $@ $^

_webrtc_audio_processing.so: webrtc_audio_processing_wrap.o audio_processing_module.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(WEBRTCLIBFILE)

clean:
	-rm -f webrtc_audio_processing_wrap.cpp *.o _webrtc_audio_processing.so webrtc_audio_processing.py *.pyc 
