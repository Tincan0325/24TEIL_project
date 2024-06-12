import mir_eval
import os
from midi2ref import note2Hz as n2Hz
import librosa    
from pydub import AudioSegment
import scipy.io.wavfile as wavfile
import numpy
import soundfile


def normalize_audio(path:str):
    def match_target_amplitude(sound, target_dBFS):
        change_in_dBFS = target_dBFS - sound.dBFS
        return sound.apply_gain(change_in_dBFS)

    def to_16bit_wav(filepath):
        data, samplerate = soundfile.read(filepath)
        soundfile.write(filepath, data, samplerate, subtype='PCM_16')
        
    sound = AudioSegment.from_file(path, sample_width=2, frame_rate=16000, channels=1)
    if (sound.sample_width != 2):
        to_16bit_wav(path)
    sound = AudioSegment.from_file(path, sample_width=2, frame_rate=16000, channels=1)
    normalized_sound = match_target_amplitude(sound, -20.0)
    normalized_sound.export(path[:-4]+'_nor.wav', format="wav")

path1 = os.path.join(os.getcwd(), 'babyslakh_16k', 'Track00005', 'piano', 'only_piano_sounds.wav')
path2 = os.path.join(os.getcwd(), 'transcribe', 'Track00005', 'piano', 'only_piano_sounds_basic_pitch.wav')

normalize_audio(path1)
normalize_audio(path2)
ref, srr,  = librosa.load(path1[:-4]+'_nor.wav', sr=16000)
est, sre = librosa.load(path2[:-4]+'_nor.wav', sr=srr)
l = min(len(est),len(ref))
ref = ref[:l]
est = est[:l]
print(ref[100000:1000000], est[100000:1000000])
print(srr, sre)
res = ref-est
wavfile.write('res.wav', srr, res)