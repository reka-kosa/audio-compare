#include "src/core/AudioProcessor.h"
#include "src/init/InitProcessing.h"

int main()
{

    InitProcessing init;
    init.InteractWithUser();
    init.CheckDirectory();

    AudioProcessor audio_proc(init.get_src_folder(), init.get_sampling_rate());
    audio_proc.StartProcessing();

    return 0;
}