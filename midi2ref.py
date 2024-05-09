import subprocess
import sys
import yaml
import os
import logging
import pandas as pd

def midi2ref(path, o_dir, TOTAL_TRACK):
    for tr in range(1, TOTAL_TRACK+1):
        cmd = f"python midi_csv/midi_to_csv.py -u {path} -o {o_dir}"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        logging.info(cmd)
        print(result.stdout)
        #print(result.stderr)

def note2Hz(note: str):
    Base = 1.05946309
    flat = 0
    name = ord(note[0])-ord('A')
    octave = int(note[-1])
    if(len(note)==3):
        flat = 1

    if (0<=name<=1): #A,B
        scale = 2*name
    elif (2<=name<4): #C,D,E
        scale = 2*name-1
        octave -= 1
    else: #F,G
        scale = 2*name-2
        octave -= 1

    scale = scale-flat
    Hz = 440.0*2**(octave-4)*Base**scale
    return Hz
    
def format(recipe, file_loc:str):
    data = pd.read_csv(file_loc)
    note = data.iloc[:, 1].to_frame()
    note_ = pd.DataFrame(columns=['Hz'])
    # convert note to Hz
    for index, n in note.iterrows():
        n_ = pd.Series(n['note_name'], dtype="string")[0]
        note_.loc[index] = note2Hz(n_)
        
    for index, row in data['duration'].to_frame().iterrows():
        l = row[0].split('/')
        if(len(l)==2):
            e = float(l[0])/float(l[1])
        else:
            e = float(l[0]) 
        # print(index)
        data.loc[index, 'endtime'] = e + data.loc[index, 'start_time']

    data = pd.concat([data, note_], axis=1)
    data = data[['start_time', 'endtime', 'Hz']]
    data = data.round(2).applymap(lambda x: "{:.{}f}".format(x, 2))
    file_loc = os.path.join('/', *file_loc.split('/')[:-1], file_loc.split('/')[-1][:-4]+'.csv')
    data.to_csv(file_loc, index=False, header=False, sep=' ')

def main(recipe:str, TOTAL_TRACK:int, RefOrTrans:str):
    with open(os.path.join(os.getcwd(), 'recipes', recipe+'.yaml')) as stream:
        recipe_data = yaml.safe_load(stream)
        file_name = next(iter(recipe_data['Recipes']))
        target = recipe_data['Recipes'][file_name]
        file_name = file_name.replace(' ', '_').lower()

    if RefOrTrans == 'ref':   
        logging.info('ref') 
        for tr in range(1, TOTAL_TRACK+1):
            with open(os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', 'metadata.yaml')) as stream:
                metadata = yaml.safe_load(stream)
                for S in metadata['stems']:
                    inst = metadata['stems'][S]['program_num']
                    if inst in target: 
                        print(f'Find inst {inst} {S} fit target')
                        path = os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', 'MIDI', S+'.mid')
                        o_dir = os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', recipe, 'insts')
                        if not os.path.exists(o_dir):
                            os.mkdir(o_dir)
                            
                        midi2ref(path=path, o_dir=o_dir, TOTAL_TRACK=TOTAL_TRACK)
                        # delete the first row of the csv file
                        for tr in range(1, TOTAL_TRACK+1):
                            file_loc = os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', recipe,  'insts', S+'.csv')
                            format(recipe=recipe, file_loc=file_loc)


    elif RefOrTrans == 'trans':
        for tr in range(1, TOTAL_TRACK+1):
            path = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.mid')
            o_dir = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe)
            midi2ref(path=path, o_dir=o_dir, TOTAL_TRACK=TOTAL_TRACK)
        # delete the first row of the csv file
        for tr in range(1, TOTAL_TRACK+1):
            file_loc = os.path.join(os.getcwd(), 'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.csv')
            format(recipe=recipe, file_loc=file_loc)

if __name__ == '__main__':
    recipe = sys.argv[1]
    TOTAL_TRACK = int(sys.argv[2])
    main(recipe, TOTAL_TRACK, 'ref')