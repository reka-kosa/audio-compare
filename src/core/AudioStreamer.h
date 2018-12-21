#ifndef _AUDIOSTREAMER_H_
#define _AUDIOSTREAMER_H_

#include <queue>
#include <string>
#include <vector>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include "Audio.h"


class AudioStreamer;

namespace audio_streamer
{
    gboolean handle_message(GstBus *bus, GstMessage *message, AudioStreamer *streamer);
    void handle_error(GstMessage *message);
    GstFlowReturn sample_arrived(GstAppSink *appsink, AudioStreamer *audio_streamer);
}

class AudioStreamer
{
    public:
        AudioStreamer(std::string src_folder, unsigned int sampling_rate);
        ~AudioStreamer();
        std::vector<Audio>* Stream();
        
    private:
        void InitGStreamer(AudioStreamer *audio_streamer);
        void GetSourceFilenames();
        void RestartPipeline();

        friend gboolean audio_streamer::handle_message(GstBus *bus, GstMessage *message, AudioStreamer *streamer);
        friend GstFlowReturn audio_streamer::sample_arrived(GstAppSink *appsink, AudioStreamer *audio_streamer);

        std::string _src_folder;
        unsigned int _sampling_rate;
        std::queue<std::string> _filenames;
        std::vector<Audio> _raw_audio_data;

        //GStreamer related
        GMainLoop *_mainloop;
        GstElement *_playbin, *_appsink; 
        guint _bus_watch_id;
        std::vector<gint16> _tmp_samples;
};

#endif