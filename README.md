# Comparing music based on acoustic features


![musickk](https://i.pinimg.com/originals/52/be/8e/52be8e782a37fe8bc3f0c30b17734898.jpg?style=centerme)  

## Required python environment for preprocessing .mp3 files
``` conda create -n mp3proc python=3.6  ``` <br>
``` conda install -n mp3proc -c menpo ffmpeg ``` <br>
``` source activate mp3proc ``` <br>
``` pip install pydub ``` <br>

Start the program from this environment.

## Required packages
Install required packages using the following command: <br>
1. **GStreamer** for streaming and decoding audio files: <br>
```sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio libgstreamer-plugins-base1.0-dev``` ,
2. **Eigen** for linear algebra and FFT: <br>
```sudo apt-get install libeigen3-dev```
3. **Boost** for handling directories: <br>
```sudo apt-get install libboost-dev```

## Build and start program
From project root using terminal,  
build with <br>
```cmake .``` <br>
```make``` <br>
run with <br>
```source activate mp3proc``` <br>
```./audio_compare```
