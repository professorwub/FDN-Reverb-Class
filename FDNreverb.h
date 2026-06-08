/*
  ==============================================================================

	FDNreverb.h
	Created: 24 Nov 2019 12:35:34pm
	Author:  profw

  ==============================================================================
*/

#pragma once

constexpr auto SPEEDOFSOUND = 343.0; // speed of sound in m/s
constexpr auto ORDER = 6;
constexpr double PHI = 1.618; // golden ratio
constexpr auto MAX_SAMPLE_DELAY = 32000; // maximum delay in samples (for delay line buffer size)

/// <summary>
/// Creates a reverb effect using a feedback delay network
/// </summary>
class FDNreverb
{
public:
	FDNreverb();
	~FDNreverb() {}

	/// <summary>
	/// Set the room area parameter.
	/// The room area is used to determine the maximum path length for the delay lines,
	/// and thus the sample delay and damping for each delay line.
	/// The path lengths are determined based on the golden rectangle,
	/// with each subsequent path length being a factor of alpha shorter than the previous one.
	/// The alpha parameter is calculated based on the number of delay lines (ORDER)
	/// to ensure a good distribution of path lengths.
	/// </summary>
	/// <param name="roomarea">room area in m^2</param>
	void setroomarea(double roomarea);

	/// <summary>
	/// Set absorption coefficient
	/// The absorption coefficient is used to determine the damping for each delay line,
	/// with higher absorption coefficients resulting in more damping.
	/// </summary>
	/// <param name="abscoef">HF absorption coefficient in dB/m</param>
	void setabscoef(double abscoef);

	/// <summary>
	/// Set reflection parameter
	/// </summary>
	/// <param name="reflect">reflection parameter (no units)</param>
	void setreflection(double reflection) { m_reflection = reflection; }

	/// <summary>
	/// Set the sampling rate
	/// </summary>
	/// <param name="samplingrate">Sampling rate in Hz</param>
	void setsamplingrate(double samplingrate) { m_samplingRate = samplingrate; }

	/// <summary>
	/// Clear all delay lines
	/// </summary>
	void reset();

	/// <summary>
	/// Step FDN reverb through one sample period
	/// This method processes a single input sample through the FDN reverb and returns the output sample.
	/// </summary>
	/// <param name="sample">input sample</param>
	/// <returns>output sample</returns>
	double step(double sample);

	/// <summary>
	/// Process a block of audio samples through the FDN reverb.
	/// This method processes an array of input samples through the FDN reverb.
	/// Each output sample is a weighted mix of the input sample and the reverb output,
	/// with the mix determined by the pctReverb parameter. The dbGain parameter allows for adjusting the overall gain of the output signal.
	/// </summary>
	/// <param name="samples">pointer to the array of input/output samples</param>
	/// <param name="numSamples">number of samples in the array</param>
	/// <param name="pctReverb">percent reverb (0-100)</param>
	/// <param name="dbGain">gain in dB</param>
	void procBlock(float* samples, unsigned numSamples, double pctReverb = 100.0, double dbGain = 0.0);

private:
	float m_delayLine[ORDER][MAX_SAMPLE_DELAY];
	unsigned m_sampleDelay[ORDER];
	double m_damping[ORDER];
	double x[ORDER];
	double m_alpha; // used to determine path lengths
	double m_samplingRate;
	double m_absCoef; // HF absorption coefficient in dB/m
	double m_reflection; // reflection parameter (no units)
	unsigned m_oldest; // index of oldest sample in delay line (for circular buffer)
};
