#include "Audio.h"
#include <iostream>
#include <chrono>
#include "MFCC.h"


Audio::Audio(std::string filepath, int sample_rate, std::vector<gint16> samples) :
        _sample_rate(sample_rate), _samples(samples)
{
    std::set<char> delims{'/'};
    _filename = SplitPath(filepath, delims);
    std::cout << "Audio track object is created for: " << _filename << std::endl;

    // Start thread
    Evaluate();
}

/// Creates probability distribution
void Audio::Evaluate()
{
    std::cout << "Evaluation is STARTED for " << _filename << std::endl;
    // 1.) Compute Mfccs for ~20ms frame windows (1024 data points) and ~10ms hop/step size (512 data point overlap)
    //     and construct _mfcc_time_series that way
    MFCC mfcc;
    mfcc.CalculateMFCCTimeSeries(*this);

    // 2.) Create parameters for multivariate normal distribution from mfcc features to represent music
    ComputeMeanVec();
    ComputeCovMat();

    // [DEBUG] Print some stuff (TODO)
    std::cout << "One row from mfcc time series: " << std::endl;
    std::cout << _mfccs_time_series.row(2) << std::endl;
    std::cout << "the mean vector of featues:" << std::endl;
    std::cout << _mean_vec << std::endl;
    std::cout << "The number of features: " << std::endl;
    std::cout << _mean_vec.rows() << std::endl;
    std::cout << "The covariance matrix of the features: " << std::endl;
    std::cout << _cov_mat << std::endl;
    std::cout << "The rows, cols of covaraince matrix: " << std::endl;
    std::cout << _cov_mat.rows() << " x " << _cov_mat.cols() << std::endl;

    // 3.) Clear unused data
    std::cout << "Evaluation is ENDED for " << _filename << std::endl;
    _samples.clear();
    _mfccs_time_series.resize(0, 0);

}

void Audio::removeColumn(Eigen::MatrixXf& matrix, unsigned int colToRemove)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-1;

    if( colToRemove < numCols )
        matrix.block(0,colToRemove,numRows,numCols-colToRemove) = matrix.rightCols(numCols-colToRemove);

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

//void Audio::removeRow(Eigen::MatrixXf& matrix, unsigned int rowToRemove)
//{
//    unsigned int numRows = matrix.rows()-1;
//    unsigned int numCols = matrix.cols();
//
//    if( rowToRemove < numRows )
//        matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.bottomRows(numRows-rowToRemove);
//
//    matrix.conservativeResize(numRows,numCols);
//}

void Audio::ComputeMeanVec()
{
    // Remove first two col, the first 2 20ms contains zeros mostly....
    removeColumn(_mfccs_time_series, 0);
    removeColumn(_mfccs_time_series, 0);

    // Replaces nans with zeros
    _mfccs_time_series = _mfccs_time_series.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    _mean_vec = _mfccs_time_series.rowwise().mean();
}

void Audio::ComputeCovMat()
{
    Eigen::MatrixXf centered = _mfccs_time_series.colwise() - _mfccs_time_series.rowwise().mean();
    _cov_mat = (centered * centered.transpose()) / float(_mfccs_time_series.cols() - 1);
}