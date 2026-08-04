/*
  ==============================================================================

    FDNreverb.cpp
    Created: 24 Nov 2019 12:35:34pm
    Author:  profw

  ==============================================================================
*/

#include "FDNreverb.h"
#include <cmath>

FDNreverb::FDNreverb()
{
	m_alpha = pow(0.1, 1.0 / (double(ORDER) - 1.0));
	m_samplingRate = 44100.0;
	m_absCoef = 0.3;
	m_reflection = 1.0;
	m_current = 0;
	for (int k = 0; k < ORDER; k++)
	{
		x[k] = 0.0;
		m_sampleDelay[k] = 0;
		m_damping[k] = 0.0;
	}
}

void FDNreverb::setroomarea(double roomarea)
{
	// Determine maximum path length based on golden rectangle
	double pathlength = sqrt(roomarea * PHI);

	// Set sample delay and damping for each delay line
	for (int k = 0; k < ORDER; k++)
	{
		m_sampleDelay[k] = lround(m_samplingRate * pathlength / SPEEDOFSOUND);
		double dBloss = m_absCoef * pathlength;
		double loss = pow(10.0, -dBloss / 20.0);
		m_damping[k] = (1.0 - loss) / (1.0 + loss);
		pathlength *= m_alpha;
	}
}

void FDNreverb::setabscoef(double abscoef)
{
	m_absCoef = abscoef;
	// Adjust damping for each delay line
	for (int k = 0; k < ORDER; k++)
	{
		double dBloss = m_absCoef * SPEEDOFSOUND * double(m_sampleDelay[k]) / m_samplingRate;
		double loss = pow(10.0, -dBloss / 20.0);
		m_damping[k] = (1.0 - loss) / (1.0 + loss);
	}
}

void FDNreverb::reset()
{
	m_current = m_delayLine[0].size() - 1;
	for (int k = 0; k < ORDER; k++)
	{
		x[k] = 0.0;
		for (auto& dline : m_delayLine[k])
			dline = 0.0f;
	}
}

double FDNreverb::step(double audioIn)
{
	// Update state variables
	for (unsigned k = 0; k < ORDER; k++)
	{
		unsigned old = (m_current + m_sampleDelay[k]) % m_delayLine[k].size();
		x[k] = m_damping[k] * x[k] + (1.0 - m_damping[k]) * m_delayLine[k][old];
	}

	// Calculate output
	double audioOut = 0.0;
	for (unsigned k = 0; k < ORDER; k++)
		audioOut += x[k];
	audioOut /= double(ORDER);

	// Update delay lines with new input sample
	for (unsigned k = 0; k < ORDER; k++)
		m_delayLine[k][m_current] = audioIn + m_reflection * audioOut - x[k];

	// Update index of oldest sample in delay line
	m_current = m_current == 0 ? m_delayLine[0].size() - 1 : m_current - 1;

	// Return output
	return audioOut;
}

void FDNreverb::procBlock(float* samples, unsigned numSamples, double pctReverb, double dbGain)
{
	double gain = pow(10.0, dbGain / 20.0);
	for (unsigned n = 0; n < numSamples; n++)
		samples[n] = float((1.0 - pctReverb / 100.0) * samples[n] + (pctReverb / 100.0) * step(samples[n])) * float(gain);
}

void FDNreverb::setmaxsampledelay(double maxroomarea, double samplingrate)
{
	// This method calculates the maximum sample delay for the delay lines based on the maximum room area and sampling rate.
	// It must be called before any audio processing occurs to ensure that the delay lines are properly sized.

	double maxpathlength = sqrt(maxroomarea * PHI);
	unsigned maxSampleDelay = lround(samplingrate * maxpathlength / SPEEDOFSOUND) + 1;
	for (int k = 0; k < ORDER; k++)
	{
		m_delayLine[k].clear();
		m_delayLine[k].resize(maxSampleDelay, 0.0f);
	}
}
