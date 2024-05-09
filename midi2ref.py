import subprocess
import sys
import yaml
import os
import logging

def midi2ref(path, o_dir, TOTAL_TRACK):
    for tr in range(1, TOTAL_TRACK+1):
        cmd = f"python midi_csv/midi_to_csv.py -u {path} -o {o_dir}"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        print(result.stdout)
        #print(result.stderr)


def main(recipe:str, TOTAL_TRACK:int, RefOrTrans:str):
    with open(os.path.join(os.getcwd(), 'recipes', recipe+'.yaml')) as stream:
        recipe_data = yaml.safe_load(stream)
        file_name = next(iter(recipe_data['Recipes']))
        target = recipe_data['Recipes'][file_name] 

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
                        o_dir = os.path.join(os.getcwd(),'babyslakh_16k', f'Track{tr:05}', recipe)
                        midi2ref(path=path, o_dir=o_dir, TOTAL_TRACK=TOTAL_TRACK)

    elif RefOrTrans == 'trans':
        for tr in range(1, TOTAL_TRACK+1):
            path = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe, file_name+'.mid')
            o_dir = os.path.join(os.getcwd(),'transcribe', f'Track{tr:05}', recipe)
            midi2ref(path=path, o_dir=o_dir, TOTAL_TRACK=TOTAL_TRACK)

if __name__ == '__main__':
    recipe = sys.argv[1]
    TOTAL_TRACK = int(sys.argv[2])
    main(recipe, TOTAL_TRACK, 'ref')