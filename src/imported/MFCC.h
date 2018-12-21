/**
 * Modified from https://github.com/Maxwell79/mfccExtractor
 * See LICENSE
 */

#ifndef _MFCC_H_
#define _MFCC_H_

#include <map>
#include <vector>

#include <complex>
#include <cmath>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <Eigen/Dense>

#include "src/core/Audio.h"

typedef std::vector<std::complex<float> > cvect;

class MFCC
{
public:
	// Methods
	static void FFT(const std::vector<float>& time_vec, cvect& freq_vec);
	static void ComputePowerSpectrum(cvect freq_dom, float* power_spectrum);
    void CalculateMFCCTimeSeries(Audio& track);

	// Constructors
	MFCC(float sample_freq=44100, int num_of_filter_banks=36, int num_coeff = 20, int NFFT=1024);

  private:
	int _n_filter_banks;  // Number of Filter Banks
	int _n_coeff;           // Number of coefficients to return
	int _NFFT;		          // Length of the FFT
	float _min_freq;	      // Minimum Frequency (Hz)
	float _max_freq;	      // Maximum Frequency (Hz)
	float _sample_freq;       // Sample Frequency

	std::vector<std::vector<float> > filter_banks; // Vector array of the filterbanks

    void InitFilterBanks();
    std::vector<float> GenerateFrameData(const std::vector<gint16>& samples, int frame_size, int start_idx);
    // Main function to obtain the MFCC's
    std::vector<float> GetMFCCS(const float* data);
};

#endif