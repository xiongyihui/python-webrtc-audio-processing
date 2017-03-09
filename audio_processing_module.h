
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
    AudioProcessingModule(int aec_type=0, bool enable_ns=true);

    string process_stream(const string& stream);
    void process_reverse_stream(const string& data);

    bool has_echo();

    bool has_voice();

    void set_system_delay(int delay);

    void set_stream_format(int rate, int channels);
    void set_reverse_stream_format(int rate, int channels);

    int vad_level();
    void set_vad_level(int level);
    int ns_level();
    void set_ns_level(int level);
    int aec_level();
    void set_aec_level(int level);

    ~AudioProcessingModule();

private:
    AudioProcessing *ap;
    StreamConfig *stream_config;
    StreamConfig *reverse_stream_config;
    int system_delay;
    int frame_size_ms;
    float *float_buffer;
    int16_t *int16_buffer;
    float *reverse_stream_buffer;
    ChannelBuffer<float> *channel_buffer;
    ChannelBuffer<float> *out_channel_buffer;
    ChannelBuffer<float> *reverse_channel_buffer;
    ChannelBuffer<float> *out_reverse_channel_buffer;
};


#endif // __AUDIO_PROCESSING_MODULE_H__
