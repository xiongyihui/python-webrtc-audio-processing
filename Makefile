

SWIG := swig

WEBRTCLIBFILE = libwebrtc-audio-processing.a

CXXFLAGS :=  -fPIC -std=c++11 -I. -I../webrtc  $(shell python-config --cflags) -DWEBRTC_LINUX -DWEBRTC_POSIX -DWEBRTC_NS_FLOAT -DNDEBUG
LDFLAGS := -shared $(shell python-config --ldflags) -lpthread
CXX := g++


all: _audio_processing_module.so

audio_processing_module_swig.cc: audio_processing_module.i
	$(SWIG) -I. -c++ -python -o $@ $^

_audio_processing_module.so: audio_processing_module_swig.o audio_processing_module.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(WEBRTCLIBFILE)

clean:
	-rm -f audio_processing_module_swig.cc *.o *.so audio_processing_module.py *.pyc 
