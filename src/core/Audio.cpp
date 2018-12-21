#include <iostream>
#include <chrono>

#include "Audio.h"
#include "src/imported/MFCC.h"


Audio::Audio(std::string filepath, int sample_rate, std::vector<gint16> samples) :
        _sample_rate(sample_rate), _samples(samples)
{
    std::set<char> delims{'/'};
    _filename = SplitPath(filepath, delims);
    std::cout << "Audio object is created for: " << _filename << std::endl;

    Evaluate();
}

/// Creates probability distribution
void Audio::Evaluate()
{
    std::cout << "Evaluation STARTED for " << _filename << std::endl;
    // 1.) Compute Mfccs for ~20ms frame windows (1024 data points) and ~10ms hop/step size (512 data point overlap)
    //     and construct _mfcc_time_series that way
    MFCC mfcc(_sample_rate);
    mfcc.CalculateMFCCTimeSeries(*this);

    // 2.) Create parameters for multivariate normal distribution from mfcc features to represent music
    ComputeMeanVec();
    ComputeCovMat();

    std::cout << "Mean vector of features:" << std::endl;
    std::cout << _mean_vec << std::endl;
    std::cout << "Number of features: " << std::endl;
    std::cout << _mean_vec.rows() << std::endl;
    std::cout << "Covariance matrix of features: " << std::endl;
    std::cout << _cov_mat << std::endl;
    std::cout << "Rows and cols of covariance matrix: " << std::endl;
    std::cout << _cov_mat.rows() << " x " << _cov_mat.cols() << std::endl;

    // 3.) Clear unused data
    std::cout << "Evaluation ENDED for " << _filename << std::endl;
    _samples.clear();
    _mfccs_time_series.resize(0, 0);

}

void Audio::removeColumn(Eigen::MatrixXf& matrix, unsigned int col_to_remove)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-1;

    if( col_to_remove < numCols )
        matrix.block(0,col_to_remove,numRows,numCols-col_to_remove) = matrix.rightCols(numCols-col_to_remove);

    matrix.conservativeResize(numRows, numCols);
}

std::string Audio::SplitPath(const std::string& str, const std::set<char> delimiters)
{
    std::vector<std::string> result;


    char const* pch = str.c_str();
    char const* start = pch;
    for(; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);
                result.push_back(str);
            }
            else
            {
                result.push_back("");
            }
            start = pch + 1;
        }
    }
    result.push_back(start);

    return result.back();
}

void Audio::ComputeMeanVec()
{
    // Remove first two cols because the first two 20ms contain zeros mostly
    removeColumn(_mfccs_time_series, 0);
    removeColumn(_mfccs_time_series, 0);

    // Replaces NaNs with zeros
    _mfccs_time_series = _mfccs_time_series.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    _mean_vec = _mfccs_time_series.rowwise().mean();
}

void Audio::ComputeCovMat()
{
    Eigen::MatrixXf centered = _mfccs_time_series.colwise() - _mfccs_time_series.rowwise().mean();
    _cov_mat = (centered * centered.transpose()) / float(_mfccs_time_series.cols() - 1);
}