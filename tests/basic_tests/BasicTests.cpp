
#include <gtest/gtest.h> // googletest header file

#include <string>
#include "src/init/InitProcessing.h"
#include "src/core/AudioStreamer.h"
#include "src/imported/AffinityPropagation.h"
#include "src/imported/MFCC.h"
#include "src/core/AudioProcessor.h"
#include "src/core/Audio.h"


TEST(InteractingWithUser, objectInitHappened)
{
    InitProcessing ip;
    ASSERT_EQ(ip.get_sampling_rate(), 0);
    ASSERT_STREQ(ip.get_src_folder().c_str(), "");
}

TEST(InteractingWithUser, wrongDirectoryPathGiven)
{
    InitProcessing ip;

    try
    {
        ip.CheckDirectory();
    }
    catch (const std::invalid_argument& exception) {
        EXPECT_STREQ(exception.what(), "Problem with given folder. Check and try again");
    }
}


void TESTComputeCovMat(const Eigen::MatrixXf& in_array, Eigen::MatrixXf& out_covmat)
{
    Eigen::MatrixXf centered = in_array.colwise() - in_array.rowwise().mean();
    Eigen::MatrixXf cov_mat = (centered * centered.transpose()) / float(in_array.cols() - 1);
}

//TEST(MathTest, covmatMatchesWithNumpyOutput)
//{
//}


void TESTComputePowerSpectrum(cvect freq_dom, float *power_spectrum)
{
    const size_t N = freq_dom.size();
    power_spectrum[0] = norm(freq_dom[0]);
    for (size_t k = 1; k < (N + 1) / 2; ++k)
        power_spectrum[k] = 2 * norm(freq_dom[k]) / N;

    if (N % 2 == 0)
        power_spectrum[N / 2] = norm(freq_dom[N / 2]);
}

//
//TEST(MathTest, computPowerSpectrum)
//{
//}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}