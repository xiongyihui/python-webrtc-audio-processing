
#ifndef __AUDIO_PROCESSING_MODULE_H__
#define __AUDIO_PROCESSING_MODULE_H__

#include <string>

namespace webrtc
{
    class AudioProcessing;
    class StreamConfig;
    template <typename T> class ChannelBuffer;
}

using namespace std;
using namespace webrtc;

class AudioProcessingModule
{
public:
    AudioProcessingModule(int aec_type=0, bool enable_ns=false, int agc_type=0, bool enable_vad=false);

    string process_stream(const string& stream);
    void process_reverse_stream(const string& data);

    bool has_echo();

    bool has_voice();

    void set_system_delay(int delay);

    void set_stream_format(int rate, int channels, int out_rate=16000, int out_channels=1);
    void set_reverse_stream_format(int rate, int channels);

    int vad_level();
    void set_vad_level(int level);
    
    int ns_level();
    void set_ns_level(int level);

    int aec_level();
    void set_aec_level(int level);

    void set_agc_level(int level);
    int agc_level();

    // Sets the target peak |level| (or envelope) of the AGC in dBFs (decibels
    // from digital full-scale). The convention is to use positive values. For
    // instance, passing in a value of -3 corresponds to -3 dBFs, or a target
    // level 3 dB below full-scale. Limited to [-31, 0].
    void set_agc_target(int dbfs);

    ~AudioProcessingModule();

private:
    static const int default_rate = 16000;
    static const int default_channels = 1;
    static const int frame_size_ms = 10;

    AudioProcessing *ap;
    StreamConfig *nearend_config;
    StreamConfig *nearend_filtered_config;
    StreamConfig *farend_config;
    int system_delay;
    float *nearend_fbuf;
    int16_t *nearend_ibuf;
    float *farend_fbuf;
    ChannelBuffer<float> *nearend_cbuf;
    ChannelBuffer<float> *nearend_filtered_cbuf;
    ChannelBuffer<float> *farend_cbuf;
};


#endif // __AUDIO_PROCESSING_MODULE_H__
