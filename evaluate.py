import mir_eval
import os
import sys
import midi2ref

# Argument: 1. recipe
def main(recipe, file_name):
    ref_loc = os.path.join(os.getcwd(), 'midi', 'csv', recipe+'.csv')
    file_loc = os.path.join(os.getcwd(), 'midi', 'csv', file_name+'.csv')
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

if __name__ == '__main__':
    recipe = sys.argv[1]
    file_name = sys.argv[2]
    main('piano', file_name)