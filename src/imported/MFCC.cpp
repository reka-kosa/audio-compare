/**
 * Modified from https://github.com/Maxwell79/mfccExtractor
 * See LICENSE
 */

#include <cfloat>
#include <assert.h>

#include<unsupported/Eigen/FFT>

#include "MFCC.h"

MFCC::MFCC(float sample_freq, int num_of_filter_banks, int num_coeff, int NFFT)
        : _n_filter_banks(num_of_filter_banks), _n_coeff(num_coeff), _NFFT(NFFT), _sample_freq(sample_freq)
{
    _min_freq = 0.f;
    _max_freq = sample_freq/2.f;
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
		chunk = GenerateFrameData(samples, _NFFT, i);

		// B.) Calculate FFT for this frame from chunk and puts results back in chunk
        cvect freq_domain;
		FFT(chunk, freq_domain);

		// C.) compute power spectrum for the input of MFCC
		float data[_NFFT/2+1];
		ComputePowerSpectrum(freq_domain, data);

		auto mfccs = GetMFCCS(data);
		// Load into container (only the first 20 coeffs, and also we leave the first)
		for (int j = 1; j < _n_coeff; ++j)
        {
		    mfcc_container.push_back(mfccs[j]);
        }

		++frame_num;
	}


    Eigen::Map<Eigen::MatrixXf> feature_rows(mfcc_container.data(), _n_coeff-1, frame_num);

    track.set_mfccs_time_series(feature_rows);
}


std::vector<float> MFCC::GenerateFrameData(const std::vector<gint16>& samples, int frame_size, int start_idx)
{
	std::vector<float> frame(frame_size);
	auto hann_window = [frame_size](int i) -> float { return 0.5 * (1 - cos(2*M_PI*i /(frame_size-1))); };

	for (int i = 0; i < frame_size; ++i)
	{
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

	// Prepare the mel scale filter bank
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