"""
This script is to preprocess audio files for C++ evaluation,
therefore invoked by a C++ code with the following command:

python preprocessing.py src_folder sampling_rate

where src_folder is the source folder containing .mp3 files to be preprocessed and sampling rate is 44100 or 22050 Hz

NOTE: pydub python package have to be installed!!!
"""

import pydub
import re
import os
import time
import sys

def preprocess_mp3(path, target_folder, audio_file, sample_rate):
 
    name, ext = os.path.splitext(audio_file)
    if ext != '.mp3':
        print('Invalid .mp3 file')
        print(name + ext + " is skipped!")
    else: 
        mp3 = pydub.AudioSegment.from_mp3(path+audio_file)

        # pydub uses milliseconds
        two_mins = 2 * 60 * 1000
        audio_len = mp3.duration_seconds * 1000     # s --> ms

        # Trim music if longer than 2 mins
        if two_mins < audio_len:
            cut = int((audio_len - two_mins)/2)
            mp3 = mp3[cut: -cut]
            print('Trimmed audio length: %f min' % (mp3.duration_seconds/60.))

        name = re.sub('[^0-9a-zA-Z]+', '_', name)

        mp3.export(target_folder+name+ext, format='mp3', codec="libmp3lame", parameters=['-ac', '1', '-ar', sample_rate, '-ab', '312', '-sample_fmt', 's16'])

def preprocess_folder(src_folder, sample_rate):

    if not src_folder.endswith('/'):
        src_folder+= '/'

    try:
        song_names = os.listdir(src_folder)
    except(OSError, FileNotFoundError):
        raise FileNotFoundError("Invalid directory: there is no such directory")
    
    start = time.time()
    
    target_folder = src_folder + 'preprocessed_' + str(sample_rate) + '/'
    print("Creating directory: " + target_folder)

    try:
        os.mkdir(target_folder)
    except(OSError, FileExistsError):
        folder_content = os.listdir(target_folder)
        # remove already existing clustering results in directory
        for item in folder_content:
            name, ext = os.path.splitext(item)
            if ext == '':
                print('Found a clustered directory! It will be removed...')
                infiles = os.listdir(target_folder + name)
                for csong in infiles:
                    os.remove(target_folder + "/" + name + "/" + csong)
                os.rmdir(target_folder + name)

        raise FileExistsError("Preprocessed directory already exists! Skipping preprocessing...")


    for idx, name in enumerate(song_names):
        print(idx+1)
        print('Preprocessing %s...' % name)
        preprocess_mp3(src_folder, target_folder, name, sample_rate)
        print('')
    print('Running time: %f min' % ((time.time()-start) / 60.))


# 3 argument is needed!
if len(sys.argv) != 3:
    print("Error! Not enough argument -> 2 argument is needed: src_folder sampling_rate")
    exit(42)
else:
    inpath = sys.argv[1]
    sampling_rate = sys.argv[2]
    preprocess_folder(inpath, sampling_rate)
