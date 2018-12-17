#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <string>
#include <sstream>
#include <vector>
#include <gst/gst.h>
#include <thread>
#include <Eigen/Core>
#include <iostream>
#include<set>

class Audio 
{
    public:
        Audio(std::string filepath, int sample_rate, std::vector<gint16> samples);
        ~Audio()
        {
        }

        std::string get_filename() { return _filename; }
        int get_sample_rate() { return _sample_rate; }
        std::vector<gint16> get_samples() { return _samples; }
        void set_mfccs_time_series(const Eigen::MatrixXf& mat) { _mfccs_time_series = mat; }
        Eigen::MatrixXf get_mean_vec() { return _mean_vec; }
        Eigen::MatrixXf get_cov_mat() { return _cov_mat; }


    private:
        void removeColumn(Eigen::MatrixXf& matrix, unsigned int colToRemove);
        std::string SplitPath(const std::string& str, const std::set<char> delimiters);
        //void removeRow(Eigen::MatrixXf& matrix, unsigned int rowToRemove);
        void ComputeMeanVec();
        void ComputeCovMat();


        std::string _filename;
        int _sample_rate; 
        std::vector<gint16> _samples;

        // we describe the music with the multivariate normal distribution of the mfccs time seriers
        Eigen::MatrixXf _mfccs_time_series;
        Eigen::MatrixXf _mean_vec;
        Eigen::MatrixXf _cov_mat;

        void Evaluate();
};

#endif
