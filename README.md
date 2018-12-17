# Comparing music based on acoustic features


![musickk](https://i.pinimg.com/originals/52/be/8e/52be8e782a37fe8bc3f0c30b17734898.jpg?style=centerme)  

## Required packages
Install required packages using the following command: <br>
1. **GStreamer** for streaming and converting audio data: <br>
```sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio libgstreamer-plugins-base1.0-dev``` , 
2. **Eigen** for linear algebra calculations: <br>
```sudo apt install libeigen3-dev```
3. **pydub** for preprocessing <br>


## Build
Build with <br> `cmake` (from root directory) using `CMakeList.txt`.

