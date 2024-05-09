import os
import logging
import yaml
import evaluate
import argparse
import subprocess
import pandas as pd
from midi2ref import main as m2r
from slakh_utils.submixes import submixes 

TOTAL_TRACK = 1

log = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

argparser = argparse.ArgumentParser()
argparser.add_argument('-s', '--submix',action='store_true', default=False)
argparser.add_argument('-p', '--predict', action='store_true', default=False)
argparser.add_argument('-o', '--overwrite', action='store_true', default=False)
argparser.add_argument('-r', '--recipe', type=str, required=True)


args = argparser.parse_args()
recipe = args.recipe

# SUBMIXING
recipe_loc = os.path.join(os.getcwd(), 'recipes', args.recipe+'.yaml')
if os.path.exists(recipe_loc): 
    log.info(f'Recipe {args.recipe} exists')
else:
    log.error(f'Recipe {args.recipe} do not exist')


if args.submix is True:
    log.info(f'Submixing recipe {args.recipe}')
    file_loc = os.path.join(os.getcwd(), 'babyslakh_16k')

    args = argparse.Namespace(
        submix_definition_file=recipe_loc, 
        input_dir=file_loc,
        src_dir=None,
        num_threads=1,
        root_dir=file_loc)

    submixes.main(args)

# PREDICTION
from basic_pitch.inference import predict_and_save

with open(recipe_loc, 'r') as stream:
    recipe_data = yaml.safe_load(stream)
    file_name = next(iter(recipe_data.get('Recipes'))).replace(' ', '_').lower()

if args.predict is not False:
    try:
        for tr in range(1, TOTAL_TRACK+1):
            path = os.path.join(os.getcwd(),'babyslakh_16k',  args.recipe+'.wav')
            predict_and_save(
                audio_path_list=path,
                output_directory=os.path.join(os.getcwd(), 'transcribe', f'Track{tr:05}', args.recipe),
                save_midi=True,
                sonify_midi=True,
                save_model_outputs=True,
                save_notes=True,
            )

    except:
        try:
            if(args.overwrite):
                log.info('Overwriting prediction.')
                
            cmd = f"./predict.sh {args.recipe} '{file_name}' {args.overwrite} {TOTAL_TRACK}"
            result = subprocess.run(cmd, capture_output=True, shell=True, text=True)
   
            if(result.returncode == 0):
                log.info('Predict and save successful.')
            elif(result.returncode == -1):
                log.info('Predict Failed.')
                print(result.stderr)
            else:
                log.info('Prediction already exists')

        except:
            log.error('Failed to predict and save.')
        pass
    
# MIDI2REF  
m2r(recipe=recipe, TOTAL_TRACK=TOTAL_TRACK, RefOrTrans='ref')
m2r(recipe=recipe, TOTAL_TRACK=TOTAL_TRACK, RefOrTrans='trans')
logging.info('Midi2ref complete')

# delete the first row of the csv file
for tr in range(1, TOTAL_TRACK+1):
    file_loc = os.path.join(os.getcwd(), 'transcribe', f'Track{tr:05}', recipe, file_name+'_basic_pitch.csv')
    data = pd.read_csv(file_loc)
    #data = data.drop(data.index[1])
    data.to_csv(file_loc, index=False)

# EVALUATION
# evaluate.main(args.recipe, TOTAL_TRACK, file_name)