#include <cstring>
#include <cstdint>
#include <cassert>

#include <dirent.h>

#include "AudioStreamer.h"
#include "Audio.h"


namespace audio_streamer {
    gboolean handle_message(GstBus *bus, GstMessage *message, AudioStreamer *streamer)
    {
        switch (GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_ERROR:
            {
                handle_error(message);
                g_main_loop_quit(streamer->_mainloop);
               
            } break;
            case GST_MESSAGE_EOS:
            {
                g_print("End-Of-Stream reached.\n");

                // The evaluation of an Audio object starts here
                Audio current_audio(streamer->_filenames.front(), streamer->_sampling_rate, streamer->_tmp_samples);
                streamer->_filenames.pop();
                streamer->_tmp_samples.clear();


                streamer->_raw_audio_data.push_back(current_audio);
                if (streamer->_filenames.empty()) 
                {
                    g_print("No more files left.\n");
                    g_main_loop_quit(streamer->_mainloop);
                }
                else 
                {
                    streamer->RestartPipeline();
                }
            } break;
            default:
                break;
        }
        return true;
    }

    void handle_error(GstMessage *message)
    {
        GError *error;
        gchar *debug_mem;
        gst_message_parse_error(message, &error, &debug_mem);
        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(message->src), error->message);
        g_printerr("Debugging information: %s\n", debug_mem ? debug_mem : "none");
        g_clear_error(&error);
        g_free(debug_mem);
    }
        
    GstFlowReturn sample_arrived(GstAppSink *appsink, AudioStreamer *audio_streamer)
    {
        if (!gst_app_sink_is_eos(appsink))
        {
            GstSample *sample = gst_app_sink_pull_sample(appsink);
            assert(sample);

            GstBuffer *buffer = gst_sample_get_buffer(sample);
            GstMapInfo mem;
            if (gst_buffer_map(buffer, &mem, GST_MAP_READ))
            {
                for ( guint i = 0; i < mem.size; i+=2)
                {
                    guint lo = mem.data[i];
                    guint hi = mem.data[i+1];
                    gint16 hilo = (hi << 8) | lo;
                    audio_streamer->_tmp_samples.push_back(hilo);
                }
            }
            gst_buffer_unmap (buffer, &mem); 
            gst_sample_unref(sample);
        }
        return GST_FLOW_OK;
    }
}

void AudioStreamer::RestartPipeline()
{
    gst_element_unlink(_playbin, _appsink);
    gst_element_set_state(_playbin, GST_STATE_NULL);
    g_print("Next song: %s\n", _filenames.front().c_str());
    g_object_set(_playbin, "uri", _filenames.front().c_str(), nullptr);
    gst_element_link(_playbin, _appsink);
    gst_element_set_state(_playbin, GST_STATE_PLAYING);
}


AudioStreamer::AudioStreamer(std::string src_folder, unsigned int sampling_rate) :
            _src_folder(src_folder), _sampling_rate(sampling_rate)
{
    g_print("AudioStreamer object is alive now.\n");
    g_print("We're gonna stream files found in folder: \n%s\n", src_folder.c_str());

    InitGStreamer(this);
    GetSourceFilenames();
    _playbin = gst_element_factory_make("playbin", "_playbin");
    assert(_playbin);

    _appsink = gst_element_factory_make("appsink", "_appsink");
    assert(_appsink);

    g_object_set(_appsink, "emit-signals", true, "sync", false, nullptr);
    g_signal_connect(_appsink, "new-sample", G_CALLBACK(audio_streamer::sample_arrived), this);
    
    g_object_set(_playbin, "uri", _filenames.front().c_str(), "audio-sink", _appsink, nullptr);

    GstBus *bus = gst_element_get_bus(_playbin);
    assert(bus);
    _bus_watch_id = gst_bus_add_watch(bus, GstBusFunc(audio_streamer::handle_message), this);
    gst_object_unref(bus);

    gst_element_link(_playbin, _appsink);
}

AudioStreamer::~AudioStreamer()
{
    g_main_loop_quit(_mainloop);

    g_source_remove(_bus_watch_id);
    gst_object_unref(_playbin);
    g_main_loop_unref(_mainloop);

    g_print("Bye...\n");
}

void AudioStreamer::InitGStreamer(AudioStreamer *audio_streamer)
{
    gst_init(nullptr, nullptr);
    _mainloop = g_main_loop_new(nullptr, false);
    assert(_mainloop);
}

void AudioStreamer::GetSourceFilenames()
{
    DIR *dir;
    struct dirent *ent;
    if ( (dir = opendir(_src_folder.c_str())) ) 
    {
        while ( (ent = readdir(dir)) ) 
        {
            if((std::strcmp(ent->d_name, ".") != 0) && (std::strcmp(ent->d_name, "..") != 0))
            {
                _filenames.push(std::string("file://")+_src_folder+std::string(ent->d_name));
            }
        }
        closedir(dir);
    } 
    else
    {
        perror("");
    }
}

std::vector<Audio>* AudioStreamer::Stream()
{
    //check if setting state to PLAYING succeeded
    g_print("Streaming started...\n");
    gst_element_set_state(_playbin, GST_STATE_PLAYING);
    g_main_loop_run(_mainloop);
    g_print("End of streaming.\n");
    gst_element_set_state(_playbin, GST_STATE_NULL);
    return &_raw_audio_data;
}
