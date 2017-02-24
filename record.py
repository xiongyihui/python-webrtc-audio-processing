"""PyAudio example: Record a few seconds of audio and save to a WAVE file."""

import pyaudio
import wave
import sys
import signal
from audio_processing_module import AudioProcessingModule as APM


if len(sys.argv) != 2:
    print('Usage: {} output.wav'.format(sys.argv[0]))
    sys.exit(1)

CHUNK = 160
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000


p = pyaudio.PyAudio()

ap = APM(False)

wf = wave.open(sys.argv[1], 'wb')
wf.setnchannels(CHANNELS)
wf.setsampwidth(p.get_sample_size(FORMAT))
wf.setframerate(RATE)

is_quit = False

def signal_handler(sig, num):
    global is_quit

    print('interrupt signal')
    is_quit = True

signal.signal(signal.SIGINT, signal_handler)

stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

print("recording >>")
while not is_quit:
    data = stream.read(CHUNK)
    data = ap.process_stream(data)

    sys.stdout.write('1' if ap.has_voice() else '0')
    sys.stdout.flush()

    wf.writeframes(data)

print("\n<< done")

wf.close()
stream.stop_stream()
stream.close()
p.terminate()



