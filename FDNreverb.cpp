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
	m_oldest = MAX_SAMPLE_DELAY - 1;
	for (int k = 0; k < ORDER; k++)
	{
		x[k] = 0.0;
		m_sampleDelay[k] = 0;
		m_damping[k] = 0.0;
		m_delayLine[k].resize(MAX_SAMPLE_DELAY);
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
	m_oldest = MAX_SAMPLE_DELAY - 1;
	for (int k = 0; k < ORDER; k++)
	{
		x[k] = 0.0;
		for (int n = 0; n < MAX_SAMPLE_DELAY; n++)
			m_delayLine[k][n] = 0.0;
	}
}

double FDNreverb::step(double audioIn)
{
	// Calculate output
	double audioOut = 0.0;
	for (unsigned k = 0; k < ORDER; k++)
		audioOut += x[k];
	audioOut /= double(ORDER);

	// Update state variables
	for (unsigned k = 0; k < ORDER; k++)
	{
		m_delayLine[k][m_oldest] = audioIn + m_reflection * audioOut - x[k];
		unsigned old = (m_oldest + m_sampleDelay[k]) % MAX_SAMPLE_DELAY;
		x[k] = m_damping[k] * x[k] + (1.0 - m_damping[k]) * m_delayLine[k][old];
	}

	// Update index of oldest sample in delay line
	m_oldest = m_oldest == 0 ? MAX_SAMPLE_DELAY - 1 : m_oldest - 1;

	// Return output
	return audioOut;
}

void FDNreverb::procBlock(float* samples, unsigned numSamples, double pctReverb, double dbGain)
{
	double gain = pow(10.0, dbGain / 20.0);
	for (unsigned n = 0; n < numSamples; n++)
	{
		// Calculate output
		double audioOut = 0.0;
		for (unsigned k = 0; k < ORDER; k++)
			audioOut += x[k];
		audioOut /= double(ORDER);

		// Update state variables
		for (unsigned k = 0; k < ORDER; k++)
		{
			m_delayLine[k][m_oldest] = samples[n] + m_reflection * audioOut - x[k];
			unsigned old = (m_oldest + m_sampleDelay[k]) % MAX_SAMPLE_DELAY;
			x[k] = m_damping[k] * x[k] + (1.0 - m_damping[k]) * m_delayLine[k][old];
		}

		// Update index of oldest sample in delay line
		m_oldest = m_oldest == 0 ? MAX_SAMPLE_DELAY - 1 : m_oldest - 1;

		// Replace input sample with weighted sum of dry and wet signal
		samples[n] = float((1.0 - pctReverb / 100.0) * samples[n] + (pctReverb / 100.0) * audioOut) * float(gain);
	}
}
