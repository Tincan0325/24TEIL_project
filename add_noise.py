import librosa
import os
import numpy as np
import soundfile as sf

file_loc = '/home/tincan/Course/third_grade/second_semester/project/TEIL_project/babyslakh_16k'
sr = 16000

for tr in range(1, 2):
    ground_truth, _ = librosa.load(os.path.join(file_loc, 'Track'+f'{tr:05}', 'mix.wav'), sr=sr)
    std = np.std(ground_truth)
    noise = np.random.normal(0, 0.1*std, size=len(ground_truth))
    noised_audio = ground_truth+noise
    sf.write(os.path.join(file_loc, 'Track'+f'{tr:05}', 'Noised.wav'), noised_audio, samplerate=sr)

