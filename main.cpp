#include "src/AudioProcessor.h"
#include "src/AudioStreamer.h"
#include "src/MFCC.h"

#include <iostream>
#include <unistd.h>
#include <limits.h>

int main()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)))
        AudioProcessor(std::string(cwd)+std::string("/preprocessed_2018-12-17_04:28:58/"));

    return 0;
}