"""PyAudio example: Record a few seconds of audio and save to a WAVE file."""

import wave
import sys
import time
from audio_processing_module import AudioProcessingModule as APM


if len(sys.argv) < 4:
    print('Usage: {} near.wav far.wav out.wav'.format(sys.argv[0]))
    sys.exit(1)

ap = APM(True)   # enable aec
chunk_ms = 10
near = wave.open(sys.argv[1], 'rb')
far = wave.open(sys.argv[2], 'rb')
out = wave.open(sys.argv[3], 'wb')
out.setnchannels(near.getnchannels())
out.setsampwidth(near.getsampwidth())
out.setframerate(near.getframerate())

print('near - rate: {}, channels: {}, length: {}'.format(
        near.getframerate(),
        near.getnchannels(),
        near.getnframes() / near.getframerate()))
print('far - rate: {}, channels: {}'.format(far.getframerate(), far.getnchannels()))

ap.set_stream_format(near.getframerate(), near.getnchannels())
ap.set_reverse_stream_format(far.getframerate(), far.getnchannels())
ap.set_aec_level(0)
ap.set_ns_level(0)

in_data_len = near.getframerate() * 10 / 1000
in_data_bytes = in_data_len * near.getsampwidth() * near.getnchannels()
out_data_len = far.getframerate() * 10 / 1000
out_data_bytes = out_data_len * far.getsampwidth() * far.getnchannels()

while True:
    in_data = near.readframes(in_data_len)
    out_data = far.readframes(out_data_len)
    if len(in_data) != in_data_bytes or len(out_data) != out_data_bytes:
        break
    ap.process_reverse_stream(out_data)
    in_data = ap.process_stream(in_data)
    sys.stdout.write('1' if ap.has_voice() else '0')
    sys.stdout.flush()

    out.writeframes(in_data)


print('done')

near.close()
far.close()
out.close()





