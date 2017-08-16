

from webrtc_audio_processing import AP


def test_ns():
    ap = AP(enable_ns=True)
    ap.set_ns_level(1)
    ap.set_stream_format(16000, 1)

    chunk = '\0\0' * 1600
    for _ in range(16):
        out = ap.process_stream(chunk)