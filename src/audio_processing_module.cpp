

#include "audio_processing_module.h"

#include "webrtc/modules/audio_processing/include/audio_processing.h"
#include "webrtc/common_audio/channel_buffer.h"

#include <iostream>
#include <string.h>


AudioProcessingModule::AudioProcessingModule(int aec_type, bool enable_ns, int agc_type, bool enable_vad)
{
    system_delay = 0;

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

    if (1 == aec_type) {
        ap->echo_control_mobile()->Enable(true);
        ap->echo_control_mobile()->set_routing_mode(webrtc::EchoControlMobile::kLoudSpeakerphone);
    } else if (2 == aec_type) {
        ap->echo_cancellation()->Enable(true);
        ap->echo_cancellation()->set_suppression_level(EchoCancellation::kLowSuppression);
    } else if (3 == aec_type) {
        // AudioProcessing::Config config;
        // config.high_pass_filter.enabled = true;
        // config.echo_canceller3.enabled = true;
        // ap->ApplyConfig(config);
    }


    if (enable_ns) {
        ap->noise_suppression()->Enable(true);
        ap->noise_suppression()->set_level(static_cast<NoiseSuppression::Level>(0));
    }

    if (agc_type) {
        ap->gain_control()->Enable(true);
        if (agc_type == 1) {
            // Adaptive Digital AGC
            ap->gain_control()->set_mode(GainControl::kAdaptiveDigital);
            ap->gain_control()->set_target_level_dbfs(30);
        } else {
            // Adaptive Analog AGC
            ap->gain_control()->set_mode(webrtc::GainControl::kAdaptiveAnalog);
            ap->gain_control()->set_analog_level_limits(0, 100);
            ap->gain_control()->set_target_level_dbfs(30);
            ap->gain_control()->set_compression_gain_db(0);
        }
    }

    if (enable_vad) {
        ap->voice_detection()->Enable(true);
        ap->voice_detection()->set_likelihood(VoiceDetection::kVeryLowLikelihood);
        ap->voice_detection()->set_frame_size_ms(frame_size_ms);
    }

    nearend_config = new StreamConfig(default_rate, default_channels, false);
    nearend_filtered_config = new StreamConfig(default_rate, default_channels, false);

    int num_frames = nearend_config->num_frames();

    nearend_fbuf = new float[num_frames * default_channels];
    nearend_ibuf = new int16_t[num_frames * default_channels];
    nearend_cbuf = new ChannelBuffer<float>(num_frames, default_channels);
    nearend_filtered_cbuf = new ChannelBuffer<float>(num_frames, default_channels);

    farend_config = new StreamConfig(default_rate, default_channels, false);

    num_frames = farend_config->num_frames();

    farend_fbuf = new float[num_frames * default_channels];
    farend_cbuf = new ChannelBuffer<float>(num_frames, default_channels);
}

void AudioProcessingModule::set_stream_format(int rate, int channels, int out_rate, int out_channels)
{
    delete nearend_fbuf;
    delete nearend_ibuf;
    delete nearend_cbuf;
    delete nearend_filtered_cbuf;

    nearend_config->set_sample_rate_hz(rate);
    nearend_config->set_num_channels(channels);

    nearend_filtered_config->set_sample_rate_hz(out_rate);
    nearend_filtered_config->set_num_channels(out_channels);

    int num_frames = nearend_config->num_frames();

    nearend_fbuf = new float[num_frames * channels];
    nearend_ibuf = new int16_t[num_frames * channels];
    nearend_cbuf = new ChannelBuffer<float>(num_frames, channels);
    nearend_filtered_cbuf = new ChannelBuffer<float>(num_frames, channels);
}

void AudioProcessingModule::set_reverse_stream_format(int rate, int channels)
{
    delete farend_fbuf;
    delete farend_cbuf;

    farend_config->set_sample_rate_hz(rate);
    farend_config->set_num_channels(channels);

    int num_frames = farend_config->num_frames();

    farend_fbuf = new float[num_frames * channels];
    farend_cbuf = new ChannelBuffer<float>(num_frames, channels);
}

string AudioProcessingModule::process_stream(const string& nearend)
{
    const int16_t *y = (const int16_t *)(nearend.data());
    int frames = nearend_config->num_frames();
    int channels = nearend_config->num_channels();

    S16ToFloat(y, frames * channels, nearend_fbuf);
    Deinterleave(nearend_fbuf, frames, channels, nearend_cbuf->channels());

    ap->ProcessStream(nearend_cbuf->channels(),
            *nearend_config,
            *nearend_filtered_config,
            nearend_filtered_cbuf->channels());

    channels = nearend_filtered_config->num_channels();

    Interleave(nearend_filtered_cbuf->channels(), frames, channels, nearend_fbuf);
    FloatToS16(nearend_fbuf, frames * channels, nearend_ibuf);

    return string((const char *)nearend_ibuf, frames * channels * sizeof(int16_t));
}

void AudioProcessingModule::process_reverse_stream(const string& farend)
{
    const int16_t *x = (const int16_t *)(farend.data());
    int frames = farend_config->num_frames();
    int channels = farend_config->num_channels();

    S16ToFloat(x, frames * channels, farend_fbuf);
    Deinterleave(farend_fbuf, frames, channels, farend_cbuf->channels());

    ap->ProcessReverseStream(farend_cbuf->channels(),
            *farend_config,
            *farend_config,
            farend_cbuf->channels());

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

int AudioProcessingModule::agc_level()
{
    return ap->gain_control()->stream_analog_level();
}

void AudioProcessingModule::set_agc_level(int level)
{
    if (level < 0 || level > 100) {
        return;
    }
    ap->gain_control()->set_stream_analog_level(level);
}

void AudioProcessingModule::set_agc_target(int dbfs)
{
    if (dbfs < 0) {
        dbfs = -dbfs;
    }

    if (dbfs > 31) {
        dbfs = 31;
    }

    ap->gain_control()->set_target_level_dbfs(dbfs);
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
    delete nearend_config;
    delete farend_config;
    delete nearend_fbuf;
    delete nearend_ibuf;
    delete farend_fbuf;
    delete nearend_cbuf;
    delete nearend_filtered_cbuf;
    delete farend_cbuf;

    delete ap;
}
