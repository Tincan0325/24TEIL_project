import ddsp_piano.synthesize_midi_file as syn
import os 
import argparse

input_file = os.path.join(os.getcwd(), 'transcribe', 'Track00005', 'piano', 'only_piano_sounds_basic_pitch.mid')
output_file = os.path.join(os.getcwd(), 'transcribe', 'Track00005', 'piano', 'only_piano_sounds_basic_pitch_1.wav')

args = argparse.Namespace(
    midi_file=input_file,
    out_file=output_file,
    duration=None,
    piano_type=3,
    ckpt='ddsp_piano/model_weights/ckpt-0'
)

args = args.parse_args()
syn.main(args)