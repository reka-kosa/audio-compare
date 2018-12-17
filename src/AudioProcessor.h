#ifndef _AUDIOPROCESSOR_H_
#define _AUDIOPROCESSOR_H_

#include <vector>

#include "AudioStreamer.h"
#include "MFCC.h"
#include "AffinityPropagation.h"

class AudioProcessor 
{

public://Methods
        AudioProcessor(std::string src_folder);


private://Methods
        void CreateSimilarityMatrix(std::vector<Audio> &track_list);
        float KullbackLeiblerBasedSimilarity(const Eigen::MatrixXf& mean1, const Eigen::MatrixXf& cov1,
                                                const Eigen::MatrixXf& mean2, const Eigen::MatrixXf& cov2);
        void SortInClusterDirectories(std::vector<Audio>* audio_tracks);

private://Members
        std::vector<std::vector<float> > _affinity_matrix;
        std::vector<int> _labels;
        std::string _src_folder;
};

#endif