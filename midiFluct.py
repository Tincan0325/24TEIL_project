import mido
import os
import itertools
from mido.midifiles.tracks import MidiTrack, merge_tracks, fix_end_of_track
from mido.midifiles.units import tick2second

mid_file = os.path.join(os.getcwd(), 'transcribe', 'Track00005', 'piano', 'only_piano_sounds_basic_pitch.mid')
csv_file = os.path.join(os.getcwd(), 'transcribe', 'Track00005', 'piano', 'only_piano_sounds_basic_pitch.csv')

### shift midi
mid = mido.MidiFile(mid_file)

target_note = 60
shift_time = 0.1

for i, msg in enumerate(mid.play()):
    if (i % 1000 == 0):
        print(i)
    if msg.type == 'note_on':
        #print('before:', msg.time)
        msg.time += shift_time
        #print('after:', msg.time)

mid.save('shifted_file.mid')

### midi to csv 
import midi2ref
midi2ref.midi2ref(os.path.join(os.getcwd(), 'shifted_file.mid'), os.getcwd())
# midi2ref.format('piano', os.path.join(os.getcwd(),'shifted_file.csv'))

### Test the score change between the original and shifted file
import mir_eval
import sys

# adjust a midi event to a specific delta time (in tick)
def Fluctuation(idx: int, dt: int, track: MidiTrack):
    # move a midi event in idx to sep position after
    def move(tr:MidiTrack, idx: int, sep: int):
        for i, msg in enumerate(tr):
            if i == idx:
                e1 = msg.copy()
                tr.remove(msg)
                tr.insert(sep+idx, e1)

    prev = track[idx]
    print(prev)
    e1 = prev.copy()
    idx += 1
    total_time = 0.0
    for i, msg in enumerate(itertools.islice(mid.play(), idx, None)):
        total_time += msg.time
        if(total_time > dt):
            if i == 0:
                e1.time += dt
                break
            else:
                track[idx].time += e1.time
                msg.time = (total_time - dt)
                e1.time = dt-(total_time-prev.time)
                move(track, idx, i)
                break
        prev = msg
        

def main(recipe, TOTAL_TRACK, file_name):
    with open(os.path.join(os.getcwd(), 'recipes', recipe+'.txt'), 'r') as f:
        line = f.readline()
        while (line):
            tr = int(line)
            print('tr', tr)
            ref_loc = os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', recipe, recipe+'.csv')
            ref_intervals, ref_pitches = mir_eval.io.load_valued_intervals(ref_loc, delimiter=',')
            est_intervals, est_pitches = mir_eval.io.load_valued_intervals(file_name, delimiter=',')
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

#main('piano', 5, csv_file)
#main('piano', 5, 'shifted_file.csv')


