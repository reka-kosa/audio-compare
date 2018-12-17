"""
This script is to preprocess audio files for C++ evaluation
"""
import pydub

import datetime
import re
import os
import time

inpath = "/home/reka/Desktop/elte_remainder/szakdoga/audio-compare/music/music_library/test"

#refactor to become callable with conversion arguments
def preprocess_mp3(path, target_folder, audio_file, sample_rate):
 
    name, ext = os.path.splitext(audio_file)
    if ext != '.mp3':
        print('Invalid .mp3 file')
    else: 
        mp3 = pydub.AudioSegment.from_mp3(path+audio_file)

        # pydub uses milliseconds
        two_mins = 2 * 60 * 1000
        audio_len = mp3.duration_seconds * 1000     # s --> ms

        # Trim music if longer than 2 mins
        if two_mins < audio_len:
            cut = int((audio_len - two_mins)/2)
            mp3 = mp3[cut: -cut]
            print(f'Trimmed audio length: {mp3.duration_seconds/60} min')

        name = re.sub('[^0-9a-zA-Z]+', '_', name)

        mp3.export(target_folder+name+ext, format='mp3', parameters=['-ac', '1', '-ar', sample_rate, '-ab', '312', '-sample_fmt', 's16'])


#mkdir: name should derive from src_folder, if exists, append date or something like this
def preprocess_folder(src_folder, sample_rate):
    
    print('src', src_folder)
    if not src_folder.endswith('/'):
        print('hey')
        src_folder+= '/'
    print('src', src_folder)
    

    song_names = os.listdir(src_folder)
    
    start = time.time()
    date_of_creation = datetime.datetime.fromtimestamp(start).strftime('%Y-%m-%d_%H:%M:%S')
    
    target_folder = os.path.dirname(os.path.dirname(src_folder))+'/preprocessed_'+date_of_creation+'/'
    os.mkdir(target_folder)

    for idx, name in enumerate(song_names):
        print(idx+1)
        print(f'Preprocessing {name}...')
        preprocess_mp3(src_folder, target_folder, name, sample_rate)
        print('')
    print(f'Running time: {(time.time()-start) / 60.} min')

preprocess_folder(inpath, '22050')
"""
output:

.
.
.
114
Preprocessing The_Space_Merchants_-_05_-_Defeat_Song.mp3 ...
Trimmed music length: 2 min

Converted mp3 info: 
array_type: h
channels: 1
dbfs: -18.914500
frame rate: 44100
frame width: 2
duration: 2.000127

-----------------------------------------
Running time: 4.641106 min
"""
