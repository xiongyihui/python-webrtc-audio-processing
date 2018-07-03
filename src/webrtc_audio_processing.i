// webrtc_audio_processing.i

%module webrtc_audio_processing

%begin %{
#define SWIG_PYTHON_STRICT_BYTE_CHAR
%}

%include "std_string.i"

%{
#include "audio_processing_module.h"
%}

%include "audio_processing_module.h"

