
#ifndef AUDIO_COMPARE_INITPROCESSING_H
#define AUDIO_COMPARE_INITPROCESSING_H

#include <iostream>

class InitProcessing
{

public://Methods
    InitProcessing();
    void InteractWithUser();
    void CheckDirectory();

    const std::string& get_src_folder() const;
    unsigned int get_sampling_rate() const;


private://Members
    std::string _src_folder;
    unsigned int _sampling_rate;

};


#endif //AUDIO_COMPARE_INITPROCESSING_H
