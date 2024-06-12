#!/usr/bin/zsh
TOTAL_TRACK=$4
for ((tr=1; tr<$TOTAL_TRACK+1; tr++)); do
    if [ ! -d "transcribe/Track${(l:5::0:)tr}" ]; then
        mkdir "transcribe/Track${(l:5::0:)tr}"
    fi
    if [ ! -d "transcribe/Track${(l:5::0:)tr}/$1" ]; then
        mkdir "transcribe/Track${(l:5::0:)tr}/$1"
        basic-pitch "transcribe/Track${(l:5::0:)tr}/$1" "babyslakh_16k/Track${(l:5::0:)tr}/$1/$2.wav" \
        --save-midi \
        --sonify-midi \
        --save-model-outputs \
        --save-note-events
    elif [ $3 = "True" ]; then
        rm -rf "transcribe/Track${(l:5::0:)tr}/$1"
        mkdir "transcribe/Track${(l:5::0:)tr}/$1"
        basic-pitch "transcribe/Track${(l:5::0:)tr}/$1" "babyslakh_16k/Track${(l:5::0:)tr}/$1/$2.wav" \
        --save-midi \
        --sonify-midi \
        --save-model-outputs \
        --save-note-events 
    fi
done
return 0
