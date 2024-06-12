import mir_eval
import os
import sys

# Argument: 1. recipe
def main(recipe, TOTAL_TRACK, file_name):
    with open(os.path.join(os.getcwd(), 'recipes', recipe+'.txt'), 'r') as f:
        line = f.readline()
        while (line):
            tr = int(line)
            print('tr', tr)
            ref_loc = os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', recipe, recipe+'.csv')
            file_loc = os.path.join(os.getcwd(), 'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.csv')
            ref_intervals, ref_pitches = mir_eval.io.load_valued_intervals(ref_loc, delimiter=',')
            est_intervals, est_pitches = mir_eval.io.load_valued_intervals(file_loc, delimiter=',')
            # scores = mir_eval.transcription.evaluate(ref_intervals=ref_intervals, 
            #                                          ref_pitches=ref_pitches, 
            #                                          ref_velocity=ref_velocity, 
            #                                          est_intervals=est_intervals,
            #                                          est_pitches= est_pitches, 
            #                                          est_velocity = est_velocity)
            scores = mir_eval.transcription.evaluate(ref_intervals=ref_intervals, \
                                                    ref_pitches=ref_pitches, \
                                                    est_intervals=est_intervals,\
                                                    est_pitches= est_pitches, \
                                                    onset_tolerance= 0.1, \
                                                    offset_ratio=0.5, \
                                                    pitch_tolerance=100) 
            print(scores)
            line = f.readline()

if __name__ == '__main__':
    recipe = sys.argv[1]
    TOTAL_TRACK = int(sys.argv[2])
    file_name = sys.argv[3]
    main(recipe, TOTAL_TRACK, file_name)