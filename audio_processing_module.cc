

#include "audio_processing_module.h"

#include "webrtc/modules/audio_processing/include/audio_processing.h"
#include "webrtc/common_audio/include/audio_util.h"
#include "webrtc/modules/audio_processing/audio_buffer.h"
// #include "webrtc/base/logging.h"

#include <iostream>
#include <string.h>


AudioProcessingModule::AudioProcessingModule(int aec_type, bool enable_ns)
{
    // rtc::LogMessage::LogToDebug(rtc::LS_INFO);

    system_delay = 8;

    Config config;
    config.Set<ExperimentalNs>(new ExperimentalNs(false));
    config.Set<Intelligibility>(new Intelligibility(false));
    config.Set<ExperimentalAgc>(new ExperimentalAgc(false));

    // if (true) {
    //     std::vector<Point> array_geometry;
    //     array_geometry.push_back(webrtc::Point(-0.05, 0, 0));
    //     array_geometry.push_back(webrtc::Point(0.05, 0, 0));
    //     config.Set<Beamforming>(
    //         new Beamforming(true,
    //                         array_geometry,
    //                         SphericalPointf(DegreesToRadians(90), 0.f, 1.f)));
    // }


    ap = AudioProcessing::Create(config);

    if (aec_type) {

        if (aec_type == 3) {
            Config extraconfig;
            // extraconfig.Set<webrtc::DelayAgnostic>(new webrtc::DelayAgnostic(true));
            // extraconfig.Set<webrtc::ExtendedFilter>(new webrtc::ExtendedFilter(true));
            extraconfig.Set<webrtc::EchoCanceller3>(new webrtc::EchoCanceller3(true));
            // extraconfig.Set<RefinedAdaptiveFilter>(new RefinedAdaptiveFilter(true));
            ap->SetExtraOptions(extraconfig);

            // AudioProcessing::Config new_config;

            // new_config.echo_canceller3.enabled = true;
            // new_config.high_pass_filter.enabled = true;
            // ap->ApplyConfig(new_config);

        }


        ap->echo_cancellation()->Enable(true);
        ap->echo_cancellation()->set_suppression_level(EchoCancellation::kLowSuppression);
        ap->echo_cancellation()->enable_metrics(true);
        ap->echo_cancellation()->enable_delay_logging(true);
    }


    if (enable_ns) {
        ap->noise_suppression()->Enable(true);
        ap->noise_suppression()->set_level(static_cast<NoiseSuppression::Level>(0));
    }

    ap->gain_control()->Enable(true);
    ap->gain_control()->set_mode(GainControl::kAdaptiveDigital);

    ap->voice_detection()->Enable(true);
    ap->voice_detection()->set_likelihood(VoiceDetection::kVeryLowLikelihood);

    frame_size_ms = 10;

    ap->voice_detection()->set_frame_size_ms(frame_size_ms);

    int rate = 16000;
    int channels = 1;

    stream_config = new StreamConfig(rate, channels, false);

    int num_frames = stream_config->num_frames();

    float_buffer = new float[num_frames * channels];
    int16_buffer = new int16_t[num_frames * channels];
    channel_buffer = new ChannelBuffer<float>(num_frames, channels);
    out_channel_buffer = new ChannelBuffer<float>(num_frames, channels);

    reverse_stream_config = new StreamConfig(rate, channels, false);

    num_frames = reverse_stream_config->num_frames();

    reverse_stream_buffer = new float[num_frames * channels];
    reverse_channel_buffer = new ChannelBuffer<float>(num_frames, channels);
    out_reverse_channel_buffer = new ChannelBuffer<float>(num_frames, channels);
}

void AudioProcessingModule::set_stream_format(int rate, int channels)
{
    delete stream_config;
    delete float_buffer;
    delete int16_buffer;
    delete channel_buffer;
    delete out_channel_buffer;

    stream_config = new StreamConfig(rate, channels, false);

    int num_frames = stream_config->num_frames();

    float_buffer = new float[num_frames * channels];
    int16_buffer = new int16_t[num_frames * channels];
    channel_buffer = new ChannelBuffer<float>(num_frames, channels);
    out_channel_buffer = new ChannelBuffer<float>(num_frames, channels);
}

void AudioProcessingModule::set_reverse_stream_format(int rate, int channels)
{
    delete reverse_stream_config;
    delete reverse_stream_buffer;
    delete reverse_channel_buffer;
    delete out_reverse_channel_buffer;

    reverse_stream_config = new StreamConfig(rate, channels, false);

    int num_frames = reverse_stream_config->num_frames();

    reverse_stream_buffer = new float[num_frames * channels];

    reverse_channel_buffer = new ChannelBuffer<float>(num_frames, channels);
    out_reverse_channel_buffer = new ChannelBuffer<float>(num_frames, channels);
}

string AudioProcessingModule::process_stream(const string& stream)
{
    const int16_t *int16_src = (const int16_t *)(stream.data());
    int frames = stream_config->num_frames();
    int channels = stream_config->num_channels();

    S16ToFloat(int16_src, frames * channels, float_buffer);

    // ap->ProcessStream(&float_buffer,
    //     *stream_config,
    //     *stream_config,
    //     &float_buffer);

    Deinterleave(float_buffer, frames, channels, channel_buffer->channels());

    ap->ProcessStream(channel_buffer->channels(),
            *stream_config,
            *stream_config,
            out_channel_buffer->channels());

    Interleave(out_channel_buffer->channels(), frames, channels, float_buffer);

    // Interleave(channel_buffer->channels(), frames, channels, float_buffer);

    FloatToS16(float_buffer, frames * channels, int16_buffer);

    return string((const char *)int16_buffer, frames * channels * 2);
}

void AudioProcessingModule::process_reverse_stream(const string& stream)
{
    const int16_t *int16_src = (const int16_t *)(stream.data());
    int frames = reverse_stream_config->num_frames();
    int channels = reverse_stream_config->num_channels();

    S16ToFloat(int16_src, frames * channels, reverse_stream_buffer);
    Deinterleave(reverse_stream_buffer, frames, channels, reverse_channel_buffer->channels());

    ap->ProcessReverseStream(reverse_channel_buffer->channels(),
            *reverse_stream_config,
            *reverse_stream_config,
            out_reverse_channel_buffer->channels());

    ap->set_stream_delay_ms(system_delay);
}

void AudioProcessingModule::set_system_delay(int delay)
{
    system_delay = delay;
}

bool AudioProcessingModule::has_echo()
{
    return ap->echo_cancellation()->stream_has_echo();
}

bool AudioProcessingModule::has_voice()
{
    return ap->voice_detection()->stream_has_voice();
}

int AudioProcessingModule::vad_level()
{
    return ap->voice_detection()->likelihood();
}

void AudioProcessingModule::set_vad_level(int level)
{
    if (level < 0 || level > 3) {
        return;
    }
    ap->voice_detection()->set_likelihood(static_cast<VoiceDetection::Likelihood>(level));
}

int AudioProcessingModule::ns_level()
{
    return ap->noise_suppression()->level();
}

void AudioProcessingModule::set_ns_level(int level)
{
    if (level < 0 || level > 3) {
        return;
    }
    ap->noise_suppression()->set_level(static_cast<NoiseSuppression::Level>(level));
}

int AudioProcessingModule::aec_level()
{
    return ap->echo_cancellation()->suppression_level();
}

void AudioProcessingModule::set_aec_level(int level)
{
    if (level < 0 || level > 2) {
        return;
    }
    ap->echo_cancellation()->set_suppression_level(static_cast<EchoCancellation::SuppressionLevel>(level));
}

AudioProcessingModule::~AudioProcessingModule()
{
    delete stream_config;
    delete reverse_stream_config;
    delete float_buffer;
    delete int16_buffer;
    delete reverse_stream_buffer;
    delete channel_buffer;
    delete out_channel_buffer;
    delete reverse_channel_buffer;
    delete out_reverse_channel_buffer;

    delete ap;
}