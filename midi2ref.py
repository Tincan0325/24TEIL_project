import subprocess
import sys
import yaml
import os
import logging
import pandas as pd
import shutil


def midi2ref(path, o_dir):
    cmd = f"python3 midi_csv/midi_to_csv.py -u {path} -o {o_dir}"
    #print(cmd)
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    #print(result.stdout)
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
    
def format(recipe, file_loc):
    data = pd.read_csv(os.path.join(*file_loc.split('/')))
    note = data.iloc[:, 1].to_frame()
    note_ = pd.DataFrame(columns=['Hz'])
    # convert note to Hz
    for index, n in note.iterrows():
        n_ = pd.Series(n['note_name'], dtype="string")[0]
        note_.loc[index] = note2Hz(n_)
        
    end = []
    for index, row in data['duration'].to_frame().iterrows():
        l = row[0].split('/')
        if(len(l)==2):
            e = float(l[0])/float(l[1])
        else:
            e = float(l[0]) 
        # print(index)
        end.append(e + data.loc[index, 'start_time'])

    data.loc[:,'endtime'] = end
    data = pd.concat([data, note_], axis=1)
    data = data[['start_time', 'endtime', 'Hz']]
    # data = data[['start_time', 'endtime', 'Hz', 'velocity']]
    data = data.round(2).astype('float')
    for index, row in data.iterrows():
        if(row['start_time'] == row['endtime']):
            data.drop(index, inplace=True)
            
    file_loc = os.path.join('/', *file_loc.split('/')[:-1], file_loc.split('/')[-1][:-4]+'.csv')
    data.to_csv(file_loc, index=False, header=False, sep=',')

def main(file_name: str):
    o_dir = os.path.join(*file_name.split('/')[:-1], 'csv')
    midi2ref(file_name, o_dir)
    format('piano', os.path.join(o_dir, file_name.split('/')[-1][:-4]+'.csv'))
    # with open(os.path.join(os.getcwd(), 'recipes', recipe+'.yaml')) as stream:
    #     recipe_data = yaml.safe_load(stream)
    #     file_name = next(iter(recipe_data['Recipes']))
    #     target = recipe_data['Recipes'][file_name]
    #     file_name = file_name.replace(' ', '_').lower()

    # if RefOrTrans == 'ref':   
    #     with open(os.path.join(os.getcwd(), 'recipes', recipe+'.txt'), 'w') as f:
    #         logging.info('ref') 
    #         for tr in range(1, TOTAL_TRACK+1):
    #             with open(os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', 'metadata.yaml')) as stream:
    #                 metadata = yaml.safe_load(stream)
    #                 InstrumentExist = False
    #                 for S in metadata['stems']:
    #                     inst = metadata['stems'][S]['program_num']
    #                     if inst in target and os.path.exists(os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', 'MIDI', S+'.mid')):
    #                         InstrumentExist = True
    #                         logging.info(f'Find inst {inst} {S} fit target')
    #                         path = os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', 'MIDI', S+'.mid')
    #                         o_dir = os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', recipe)
    #                         o_file = os.path.join(o_dir, recipe+'.mid')
    #                         shutil.copy(path, o_file)
    #                         midi2ref(path=o_file, o_dir=o_dir)
                            
    #                         # os.rename(os.path.join(o_dir, S+'.csv'), os.path.join(o_dir, recipe+'.csv'))
    #                         # delete the first row of the csv file

    #                         file_loc = os.path.join(os.getcwd(), 'babyslakh_16k', f'Track{tr:05}', recipe,  recipe+'.csv')
    #                         format(recipe=recipe, file_loc=file_loc)

    #                 if not InstrumentExist:
    #                     logging.info(f'No instrument fit target')
    #                 else:
    #                     f.write(str(tr)+'\n')


    # elif RefOrTrans == 'trans':
    #     with open(os.path.join(os.getcwd(), 'recipes', recipe+'.txt'), 'r') as f:
    #         for tr in range(1, TOTAL_TRACK+1):
    #             path = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.mid')
    #             o_dir = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe)
    #             midi2ref(path=path, o_dir=o_dir)
    #             file_loc = os.path.join(os.getcwd(), 'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.csv')
    #             format(recipe=recipe, file_loc=file_loc)
             
if __name__ == '__main__':
    file = sys.argv[1]
    main(file)