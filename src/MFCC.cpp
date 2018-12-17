#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <map>
#include <cmath>
#include "MFCC.h"
#include <chrono>
#include <cfloat>
#include <assert.h>
#include<unsupported/Eigen/FFT>

#define _USE_MATH_DEFINES

MFCC::MFCC(int num_of_filter_banks, int num_coeff, int NFFT, float min_freq, float max_freq, float sample_freq)
        : _n_filter_banks(num_of_filter_banks), _n_coeff(num_coeff), _NFFT(NFFT), _min_freq(min_freq), _max_freq(max_freq), _sample_freq(sample_freq)
{
	InitFilterBanks();
}

void MFCC::CalculateMFCCTimeSeries(Audio& track)
{
    std::vector<float> mfcc_container;
    std::vector<float> chunk;

    // 1.) Get the samples from audio track
    auto samples = track.get_samples();

	// 2.) loop over sample data that comes from Streamer with window size _NFFT and step by  _NFFT/2
    int frame_num = 0;
	for (int i = 0; i < samples.size(); i+=_NFFT/2)
	{

//		auto start = std::chrono::system_clock::now();
		// A.) generate complex input data for FFT from samples that is _NFFT long (Hanning applied!)
		chunk = GenerateFrameData(samples, _NFFT, i);
//		auto end = std::chrono::system_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		std::cout << "1. Chunk generation: " << elapsed.count() << '\n';


//		start = std::chrono::system_clock::now();
		// B.) Calculate FFT for this frame from chunk and puts results back in chunk
        cvect freq_domain;
		FFT(chunk, freq_domain);
//		end = std::chrono::system_clock::now();
//		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		std::cout << "2. FFT: " << elapsed.count() << '\n';

//		start = std::chrono::system_clock::now();
		// C.) compute power spectrum for the input of MFCC
		float data[_NFFT/2+1];
		ComputePowerSpectrum(freq_domain, data);
//		end = std::chrono::system_clock::now();
//		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		std::cout << "3. Power spect: " << elapsed.count() << '\n';


//		start = std::chrono::system_clock::now();
		// D.) calculate the MFCCs for the data and add them to the mfcc container that holds all coeffiecents
		auto mfccs = GetMFCCS(data);
//		end = std::chrono::system_clock::now();
//		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		std::cout << "4. MFCC calc: " << elapsed.count() << '\n';

		// Load into container (only the first 20 coeff and also the first is excluded!)
		for (int j = 1; j < _n_coeff; ++j)
        {
		    mfcc_container.push_back(mfccs[j]);
        }

		++frame_num;
	}


    Eigen::Map<Eigen::MatrixXf> feature_rows(mfcc_container.data(), _n_coeff-1, frame_num);

	// [DEBUG]
    // Plot constructed mffcc matrix
    //std::cout << "Debug MFCC: " << std::endl;
	//std::cout << feature_rows.row(2) << std::endl;
    track.set_mfccs_time_series(feature_rows);
}


std::vector<float> MFCC::GenerateFrameData(const std::vector<gint16>& samples, int frame_size, int start_idx)
{
	std::vector<float> frame(frame_size);
	//maybe this hann_window should be somewhere else
	//1013 is win_size-1
	auto hann_window = [frame_size](int i) -> float { return 0.5 * (1 - cos(2*M_PI*i /(frame_size-1))); };

	for (int i = 0; i < frame_size; ++i)
	{
		//here we can throw away the first 512 input if we wish to decrease memory usage - at the moment i'm not throwing anything
		float sample = hann_window(i) * samples[i+start_idx];
		frame[i] = sample;
	}
	return frame;
}


void MFCC::FFT(const std::vector<float>& time_vec, cvect& freq_vec)
{
    Eigen::FFT<float> fft;
    fft.fwd(freq_vec, time_vec);
}

void MFCC::ComputePowerSpectrum(cvect freq_dom, float *power_spectrum)
{
	const size_t N = freq_dom.size();
	power_spectrum[0] = norm(freq_dom[0]);
	for (size_t k = 1; k < (N + 1) / 2; ++k)
		power_spectrum[k] = 2 * norm(freq_dom[k]) / N;

	if (N % 2 == 0)
		power_spectrum[N / 2] = norm(freq_dom[N / 2]);
}

void MFCC::InitFilterBanks()
{
	int nspec = _NFFT / 2 + 1;
	float min_mel = 1127.01048 * log(1.0 + _min_freq / 700.0);
	float max_mel = 1127.01048 * log(1.0 + _max_freq / 700.0);
	float d_mel = (max_mel - min_mel) / (_n_filter_banks + 1);
	std::vector<float> mel_spacing;
	std::vector<float> mel_freq;

	std::vector<float> lower;
	std::vector<float> center;

	// Init mel_spacing
	for (int i = 0; i < _n_filter_banks + 2; i++)
	{
		float mel = min_mel + i * d_mel;
		mel_spacing.push_back(mel);
	}

	// Init mel_freq
	for (int i = 0; i < nspec; i++)
	{
		float f = i * _sample_freq / _NFFT;
		float melf = log(1 + f / 700) * 1127.01048;
		mel_freq.push_back(melf);
	}

	// Init lower
	for (int i = 0; i < _n_filter_banks; i++)
	{
		lower.push_back(mel_spacing[i]);
	}

	// Init center
	for (int i = 1; i < _n_filter_banks + 1; i++)
	{
		center.push_back(mel_spacing[i]);
	}

	// Prepare the mel scale filterbank
	for (int i = 0; i < _n_filter_banks; i++)
	{
		std::vector<float> f_bank;
		for (int j = 0; j < nspec; j++)
		{
			float val = std::max(0.0f, (1 - std::abs(mel_freq[j] - center[i]) / (center[i] - lower[i])));
			f_bank.push_back(val);
		}
		filter_banks.push_back(f_bank);
	}
}

std::vector<float> MFCC::GetMFCCS(const float *data)
{

	std::vector<float> mel_spectrum;
	std::vector<float> pre_DCT;  // Initilise pre-discrete cosine transformation vector array
	std::vector<float> post_DCT; // Initilise post-discrete cosine transformation vector array / MFCC Coefficents

	// Map the spectrum to the mel scale (apply triangular filters)
	// For each filter bank (i.e. for each mel frequency)
	for (auto &it : filter_banks)
	{
		float cel = 0;
		int n = 0;
		// For each frequency in the original spectrum
		for (auto &it2 : it)
		{
			cel += it2 * data[n++];
		}

		mel_spectrum.push_back(cel);
		pre_DCT.push_back(log10(cel)); // Compute the log of the mel-frequency spectrum
	}

	// Perform the Discrete Cosine Transformation
	for (size_t i = 0; i < filter_banks.size(); i++)
	{
		float val = 0;
		for (size_t j = 0; j < pre_DCT.size(); j++)
		{
			val += pre_DCT[j] * cos(i * (j + 0.5) * M_PI / filter_banks.size());
		}

		// Perform scaling used by matlab implementation of dct
		if (i == 0)
		{
			val /= sqrt(2.0);
		}
		val *= sqrt(2.0 / filter_banks.size());

		post_DCT.push_back(val);
	}

	return post_DCT;
}