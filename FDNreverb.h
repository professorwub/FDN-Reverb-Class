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
	/// </summary>
	/// <param name="roomarea">room area in m^2</param>
	void setroomarea(double roomarea);

	/// <summary>
	/// Set absorption coefficient
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
	/// </summary>
	/// <param name="sample">input sample</param>
	/// <returns>output sample</returns>
	double step(double sample);

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