#include "AudioProcessor.h"
#include <chrono>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <cstdlib>

AudioProcessor::AudioProcessor(std::string src_folder) : _src_folder(src_folder)
{
    std::cout << "AudioProcessor" << std::endl;

    auto start = std::chrono::system_clock::now();
    ////1. get data and process it
    AudioStreamer streamer(src_folder);
    std::vector<Audio>* audio_tracks = streamer.Stream();
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "1. Processing of audio files took: " << elapsed.count()/60. << " min" << '\n';

    // Wait until evaluation processes ended in Audio objects
//    for(auto& track : *audio_tracks)
//    {
//        std::cout << track.get_filename() << std::endl;
//        std::cout << track.get_samples().size() << std::endl;
//    }
    ////2. distant function / measure / metrics
    CreateSimilarityMatrix(*audio_tracks);


    ////3. group / cluster
    AP::AffinityPropagation ap;
    ap.Fit(_affinity_matrix);

    ////4. output data
    ap.PrintLabels();
    _labels = ap.get_labels();
    SortInClusterDirectories(audio_tracks);

}


/**
 * Creates similarity matrix for the music tracks. The result is saved in @var _affinity_matrix class member.
 *
 * @param track_list A vector of Audio objects in processed state -> mean vector and covariance matrix is assigned
 */
void AudioProcessor::CreateSimilarityMatrix(std::vector<Audio> &track_list)
{
    unsigned long n = track_list.size();

    std::vector<std::vector<float> > affinity_matrix(n, std::vector<float>(n));

    // The diagonal of the similarity matrix is 1.
    for(int i = 0; i < n; ++i)
    {
        affinity_matrix[i][i] = 1.f;
    }

    // The similarity matrix is symmetric, it is more efficient to iterate over the upper triangle
    for(int i = 0; i<n-1; ++i)
    {
        for(int j = i+1; j<n; ++j)
        {
            float tmp = KullbackLeiblerBasedSimilarity(track_list[i].get_mean_vec(), track_list[i].get_cov_mat(),
                                                        track_list[j].get_mean_vec(), track_list[j].get_cov_mat());
            affinity_matrix[i][j] = tmp;
            affinity_matrix[j][i] = tmp;
        }
    }

    _affinity_matrix = affinity_matrix;
}


/**
 * Calculates a symmetric KL based similarity measure measure:
 *
 * dist = trace(cov1*invcov2) + trace(cov2*invcov1) - 2*d + trace((invcov1+invcov2)*(m1-m2)*(m1-m2)’);
 * Than this distance measure is transformed to be similarity measure as follows:
 * sim = exp(-1/fact*dist), where fact is a parameter chosen to be fact=450
 *
 * @param mean1 the mean vector of the first distreibution
 * @param cov1 the covariance matrix of the first dist.
 * @param mean2 the mean vector of the second distribution
 * @param cov2 the covariance matrix of the second dist.
 * @return the similarity value between distributions in range [0, 1] (1: maximum similarity, 0: no similarity)
 */
float AudioProcessor::KullbackLeiblerBasedSimilarity(const Eigen::MatrixXf& mean1, const Eigen::MatrixXf& cov1,
                                                const Eigen::MatrixXf& mean2, const Eigen::MatrixXf& cov2)
{

    unsigned int d = mean1.size();              // dimension of the distrbutions
    Eigen::VectorXf devmean = mean1 - mean2;
    Eigen::MatrixXf invcov1 = cov1.inverse();
    Eigen::MatrixXf invcov2 = cov2.inverse();

    float dist = (cov1*invcov2).trace() + (cov2*invcov1).trace() - 2*d +
                                        ((invcov1+invcov2)*devmean.dot(devmean)).trace();


    return std::exp(-1.f/450.f*dist);;
}

void AudioProcessor::SortInClusterDirectories(std::vector<Audio>* audio_tracks)
{
    // 1.) create directories according to unique labels in the source directory
    std::vector<int> unique_labels = _labels;
    std::sort(unique_labels.begin(), unique_labels.end());
    auto last = std::unique(unique_labels.begin(), unique_labels.end());
    unique_labels.erase(last, unique_labels.end());
    std::cout << "Unique cluster labels: " << std::endl;
    for (int i : unique_labels)
        std::cout << i << " ";
    std::cout << "\n";

    // Creating a directory
    for(auto label : unique_labels)
    {
        std::stringstream ss;
        ss << label;
        if (mkdir((_src_folder + ss.str()).c_str(), 0777) == -1)
            std::cerr << "Error :  " << strerror(errno) << std::endl;
        else
            std::cout << "Directory created: "  << (_src_folder + ss.str()) << std::endl;
    }

    // 2.) Copy music into directories according to the computed labels
    int idx = 0;
    for(auto& track : *audio_tracks)
    {
        std::stringstream ss;
        ss << _labels[idx];
        std::string src = _src_folder + track.get_filename();
        std::string dest = _src_folder + ss.str();
        std::string cp_command = "cp " + src + " " + dest;

        const int dir_err = system(cp_command.c_str());
        if (-1 == dir_err)
            std::cerr << "Error in copying files!" << std::endl;
        else
            std::cout << "Music: " << track.get_filename() << " is succefully copied into subdirectory " << ss.str() << std::endl;

        ++idx;
    }
}