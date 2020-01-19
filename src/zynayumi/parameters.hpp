/****************************************************************************
    
    Parameters for Zynayumi

    parameters.hpp

    Copyleft (c) 2016-2019 Nil Geisweiller <ngeiswei@gmail.com>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef __ZYNAYUMI_PARAMETERS_HPP
#define __ZYNAYUMI_PARAMETERS_HPP

#include <string>
#include <cmath>
#include <limits>
#include <vector>

#include "patch.hpp"

namespace zynayumi {

/**
 * Affine transformation, based on the equality
 *
 * (y - miny) / (maxy - miny) = (x - minx) / (maxx - minx)
 * y = ((x - minx) / (maxx - minx)) * (maxy - miny) + miny
 */
static float affine(float minx, float maxx, float miny, float maxy, float x) {
	return ((x - minx) / (maxx - minx)) * (maxy - miny) + miny;
}

/**
 * Class to hold user parameter, name, value and other features
 * depending on the parameter type.
 */
class Parameter {
public:
	// Ctor
	Parameter(const std::string& name);

	// Convert parameter (name and value) into string
	std::string to_string(std::string indent=std::string()) const;

	// Convert value parameter into string
	virtual std::string value_to_string() const = 0;

	// Convert value into float
	virtual float float_value() const = 0;

	// Set value given a float
	virtual void set_value(float f) = 0;

	// Convert value to normalized [0, 1] float
	virtual float norm_float_value() const = 0;

	// Set value given normalized float within [0, 1]
	virtual void set_norm_value(float nf) = 0;

	// Name exposed to the user
	std::string name;
};

class BoolParameter : public Parameter {
public:
	// Ctor
	BoolParameter(const std::string& name, bool* value_ptr, bool value_dflt);

	// Convert value parameter into string
	std::string value_to_string() const override;

	// Get/set methods
	float float_value() const override;
	void set_value(float f) override;
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Current value
	bool* value_ptr;
};

class IntParameter : public Parameter {
public:
	// Ctor
	IntParameter(const std::string& name, int* value_ptr,
	             int value_dflt, int low, int up);

	// Convert value parameter into string
	std::string value_to_string() const override;

	// Get/set methods
	float float_value() const override;
	void set_value(float f) override;
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Current value
	int* value_ptr;

	// Range [low, up]
	int low;
	int up;
};

class FloatParameter : public Parameter {
public:
	// Ctor
	FloatParameter(const std::string& name, float* value_ptr,
	               float value_dflt, float low, float up);

	// Convert value parameter into string
	std::string value_to_string() const override;

	// Get/set methods
	float float_value() const override;
	void set_value(float f) override;

	// Current value
	float* value_ptr;

	// Range [low, up]
	float low;
	float up;
};

class LinearFloatParameter : public FloatParameter {
public:
	// Ctor
	LinearFloatParameter(const std::string& name, float* value_ptr,
	                     float value_dflt, float low, float up);

	// Get/set methods
	float norm_float_value() const override;
	void set_norm_value(float nf) override;
};

class TanFloatParameter : public FloatParameter {
public:
	// Ctor
	TanFloatParameter(const std::string& name, float* value_ptr,
	                  float value_dflt, float low, float up);

	// Get/set methods
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Arc tangent range [atan_low, atan_up]
	float atan_low;
	float atan_up;
};

template<typename E>
class EnumParameter : public Parameter {
public:
	// Ctor
	EnumParameter(const std::string& name, E* value_ptr, E value_dflt);

	// Convert value parameter into string
	std::string value_to_string() const override;

	// Get/set methods
	float float_value() const override;
	void set_value(float f) override;
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Current value
	E* value_ptr;
};

template<typename E>
EnumParameter<E>::EnumParameter(const std::string& nm, E* vl_ptr, E vl_dflt)
	: Parameter(nm), value_ptr(vl_ptr)
{
	*value_ptr = vl_dflt;
}

template<typename E>
std::string EnumParameter<E>::value_to_string() const
{
	return zynayumi::to_string(*value_ptr);
}

template<typename E>
float EnumParameter<E>::float_value() const
{
	return (float)*value_ptr;
}

template<typename E>
void EnumParameter<E>::set_value(float nf)
{
	*value_ptr = (E)std::round(nf);
}

template<typename E>
float EnumParameter<E>::norm_float_value() const
{
	return (float)*value_ptr / (float)((int)E::Count - 1);
}

template<typename E>
void EnumParameter<E>::set_norm_value(float nf)
{
	*value_ptr = (E)std::round(nf * (float)((int)E::Count - 1));
}

// Parameter indices
enum ParameterIndex {
	// Play mode
	PLAY_MODE,

	// Tone
	TONE_TIME,
	TONE_DETUNE,
	TONE_TRANSPOSE,

	// Noise
	NOISE_TIME,
	NOISE_PERIOD,

	// Noise Period Envelope
	NOISE_PERIOD_ENV_ATTACK,
	NOISE_PERIOD_ENV_TIME,

	// Amplitude envelope
	AMP_ENV_ATTACK_LEVEL,
	AMP_ENV_TIME1,
	AMP_ENV_LEVEL1,
	AMP_ENV_TIME2,
	AMP_ENV_LEVEL2,
	AMP_ENV_TIME3,
	AMP_ENV_SUSTAIN_LEVEL,
	AMP_ENV_RELEASE,

	// Pitch envelope
	PITCH_ENV_ATTACK_PITCH,
	PITCH_ENV_TIME,

	// Arpegio
	ARP_PITCH1,
	ARP_PITCH2,
	ARP_PITCH3,
	ARP_FREQ,
	ARP_REPEAT,

	// Ring modulation
	RING_MOD_WAVEFORM_LEVEL1,
	RING_MOD_WAVEFORM_LEVEL2,
	RING_MOD_WAVEFORM_LEVEL3,
	RING_MOD_WAVEFORM_LEVEL4,
	RING_MOD_WAVEFORM_LEVEL5,
	RING_MOD_WAVEFORM_LEVEL6,
	RING_MOD_WAVEFORM_LEVEL7,
	RING_MOD_WAVEFORM_LEVEL8,
	RING_MOD_MIRROR,
	RING_MOD_SYNC,
	RING_MOD_DETUNE,
	RING_MOD_TRANSPOSE,

	// Pitch LFO
	LFO_FREQ,
	LFO_DELAY,
	LFO_DEPTH,

	// Portamento time
	PORTAMENTO_TIME,

	// Pan
	PAN_CHANNEL0,
	PAN_CHANNEL1,
	PAN_CHANNEL2,

	// Pitch wheel range
	PITCH_WHEEL,

	// Emulation mode (YM2149 vs AY-3-8910)
	EMUL_MODE,

	// Number of Parameters
	PARAMETERS_COUNT
};

// Parameter names
#define PLAY_MODE_STR "Play mode"
#define TONE_TIME_STR "Tone time"
#define TONE_DETUNE_STR "Tone detune"
#define TONE_TRANSPOSE_STR "Tone transpose"
#define NOISE_TIME_STR "Noise time"
#define NOISE_PERIOD_STR "Noise period"
#define NOISE_PERIOD_ENV_ATTACK_STR "NoisePeriodEnv attack"
#define NOISE_PERIOD_ENV_TIME_STR "NoisePeriodEnv time"
#define AMP_ENV_ATTACK_LEVEL_STR "AmpEnv attack level"
#define AMP_ENV_TIME1_STR "AmpEnv time1"
#define AMP_ENV_LEVEL1_STR "AmpEnv level1"
#define AMP_ENV_TIME2_STR "AmpEnv time2"
#define AMP_ENV_LEVEL2_STR "AmpEnv level2"
#define AMP_ENV_TIME3_STR "AmpEnv time3"
#define AMP_ENV_SUSTAIN_LEVEL_STR "AmpEnv sustain level"
#define AMP_ENV_RELEASE_STR "AmpEnv release"
#define PITCH_ENV_ATTACK_PITCH_STR "PitchEnv attack pitch"
#define PITCH_ENV_TIME_STR "PitchEnv time"
#define ARP_PITCH1_STR "Arp pitch1"
#define ARP_PITCH2_STR "Arp pitch2"
#define ARP_PITCH3_STR "Arp pitch3"
#define ARP_FREQ_STR "Arp freq"
#define ARP_REPEAT_STR "Arp repeat"
#define RING_MOD_WAVEFORM_LEVEL1_STR "RingMod waveform level1"
#define RING_MOD_WAVEFORM_LEVEL2_STR "RingMod waveform level2"
#define RING_MOD_WAVEFORM_LEVEL3_STR "RingMod waveform level3"
#define RING_MOD_WAVEFORM_LEVEL4_STR "RingMod waveform level4"
#define RING_MOD_WAVEFORM_LEVEL5_STR "RingMod waveform level5"
#define RING_MOD_WAVEFORM_LEVEL6_STR "RingMod waveform level6"
#define RING_MOD_WAVEFORM_LEVEL7_STR "RingMod waveform level7"
#define RING_MOD_WAVEFORM_LEVEL8_STR "RingMod waveform level8"
#define RING_MOD_MIRROR_STR "RingMod mirror"
#define RING_MOD_SYNC_STR "RingMod sync"
#define RING_MOD_DETUNE_STR "RingMod detune"
#define RING_MOD_TRANSPOSE_STR "RingMod transpose"
#define LFO_FREQ_STR "LFO freq"
#define LFO_DELAY_STR "LFO delay"
#define LFO_DEPTH_STR "LFO depth"
#define PORTAMENTO_TIME_STR "Portamento time"
#define PAN_CHANNEL0_STR "Pan channel0"
#define PAN_CHANNEL1_STR "Pan channel1"
#define PAN_CHANNEL2_STR "Pan channel2"
#define PITCH_WHEEL_STR "Pitch wheel"
#define EMUL_MODE_STR "Emulation mode"

// Parameter defaults
#define PLAY_MODE_DFLT PlayMode::Mono
#define TONE_TIME_DFLT std::numeric_limits<float>::infinity()
#define TONE_DETUNE_DFLT 0.0
#define TONE_TRANSPOSE_DFLT 0
#define NOISE_TIME_DFLT 0.0
#define NOISE_PERIOD_DFLT 1
#define NOISE_PERIOD_ENV_ATTACK_DFLT 1
#define NOISE_PERIOD_ENV_TIME_DFLT 0.0
#define AMP_ENV_ATTACK_LEVEL_DFLT 1.0
#define AMP_ENV_TIME1_DFLT 0.0
#define AMP_ENV_LEVEL1_DFLT 1.0
#define AMP_ENV_TIME2_DFLT 0.0
#define AMP_ENV_LEVEL2_DFLT 1.0
#define AMP_ENV_TIME3_DFLT 0.0
#define AMP_ENV_SUSTAIN_LEVEL_DFLT 1.0
#define AMP_ENV_RELEASE_DFLT 0.0
#define PITCH_ENV_ATTACK_PITCH_DFLT 0.0
#define PITCH_ENV_TIME_DFLT 0.0
#define ARP_PITCH1_DFLT 0
#define ARP_PITCH2_DFLT 0
#define ARP_PITCH3_DFLT 0
#define ARP_FREQ_DFLT 12.5
#define ARP_REPEAT_DFLT 0
#define RING_MOD_WAVEFORM_LEVEL1_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL2_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL3_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL4_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL5_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL6_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL7_DFLT 1.0
#define RING_MOD_WAVEFORM_LEVEL8_DFLT 1.0
#define RING_MOD_MIRROR_DFLT true
#define RING_MOD_SYNC_DFLT true
#define RING_MOD_DETUNE_DFLT 0.0
#define RING_MOD_TRANSPOSE_DFLT 0
#define LFO_FREQ_DFLT 1.0
#define LFO_DELAY_DFLT 0.0
#define LFO_DEPTH_DFLT 0.0
#define PORTAMENTO_TIME_DFLT 0.0
#define PAN_CHANNEL0_DFLT 0.5
#define PAN_CHANNEL1_DFLT 0.25
#define PAN_CHANNEL2_DFLT 0.75
#define PITCH_WHEEL_DFLT 2
#define EMUL_MODE_DFLT EmulMode::YM2149

// Parameter ranges
#define TONE_TIME_L 0.0f
#define TONE_TIME_U std::numeric_limits<float>::infinity()
#define TONE_DETUNE_L -0.5f
#define TONE_DETUNE_U 0.5f
#define TONE_TRANSPOSE_L -24
#define TONE_TRANSPOSE_U 24
#define NOISE_TIME_L 0.0f
#define NOISE_TIME_U std::numeric_limits<float>::infinity()
#define NOISE_PERIOD_L 1
#define NOISE_PERIOD_U 31
#define NOISE_PERIOD_ENV_ATTACK_L 1
#define NOISE_PERIOD_ENV_ATTACK_U 31
#define NOISE_PERIOD_ENV_TIME_L 0.0f
#define NOISE_PERIOD_ENV_TIME_U 5.0f
#define AMP_ENV_ATTACK_LEVEL_L 0.0f
#define AMP_ENV_ATTACK_LEVEL_U 1.0f
#define AMP_ENV_TIME1_L 0.0f
#define AMP_ENV_TIME1_U 5.0f
#define AMP_ENV_LEVEL1_L 0.0f
#define AMP_ENV_LEVEL1_U 1.0f
#define AMP_ENV_TIME2_L 0.0f
#define AMP_ENV_TIME2_U 5.0f
#define AMP_ENV_LEVEL2_L 0.0f
#define AMP_ENV_LEVEL2_U 1.0f
#define AMP_ENV_TIME3_L 0.0f
#define AMP_ENV_TIME3_U 5.0f
#define AMP_ENV_SUSTAIN_LEVEL_L 0.0f
#define AMP_ENV_SUSTAIN_LEVEL_U 1.0f
#define AMP_ENV_RELEASE_L 0.0f
#define AMP_ENV_RELEASE_U 5.0f
#define PITCH_ENV_ATTACK_PITCH_L -96.0f
#define PITCH_ENV_ATTACK_PITCH_U 96.0f
#define PITCH_ENV_TIME_L 0.0f
#define PITCH_ENV_TIME_U 5.0f
#define ARP_PITCH1_L -48
#define ARP_PITCH1_U 48
#define ARP_PITCH2_L -48
#define ARP_PITCH2_U 48
#define ARP_PITCH3_L -48
#define ARP_PITCH3_U 48
#define ARP_FREQ_L 0.0f
#define ARP_FREQ_U 50.0f
#define ARP_REPEAT_L 0.0f
#define ARP_REPEAT_U 2.0f
#define RING_MOD_WAVEFORM_LEVEL1_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL1_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL2_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL2_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL3_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL3_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL4_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL4_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL5_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL5_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL6_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL6_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL7_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL7_U 1.0f
#define RING_MOD_WAVEFORM_LEVEL8_L 0.0f
#define RING_MOD_WAVEFORM_LEVEL8_U 1.0f
#define RING_MOD_MIRROR_L 0.0f
#define RING_MOD_MIRROR_U 1.0f
#define RING_MOD_SYNC_L 0.0f
#define RING_MOD_SYNC_U 1.0f
#define RING_MOD_DETUNE_L -0.5f
#define RING_MOD_DETUNE_U 0.5f
#define RING_MOD_TRANSPOSE_L -24
#define RING_MOD_TRANSPOSE_U 24
#define LFO_FREQ_L 0.0f
#define LFO_FREQ_U 20.0f
#define LFO_DELAY_L 0.0f
#define LFO_DELAY_U 10.0f
#define LFO_DEPTH_L 0.0f
#define LFO_DEPTH_U 12.0f
#define PORTAMENTO_TIME_L 0.0f
#define PORTAMENTO_TIME_U 0.5f
#define PAN_CHANNEL0_L 0.0f
#define PAN_CHANNEL0_U 1.0f
#define PAN_CHANNEL1_L 0.0f
#define PAN_CHANNEL1_U 1.0f
#define PAN_CHANNEL2_L 0.0f
#define PAN_CHANNEL2_U 1.0f
#define PITCH_WHEEL_L 1
#define PITCH_WHEEL_U 12

class Zynayumi;

class Parameters {
public:
	// CTor, DTor
	Parameters(Zynayumi& zynayumi);
	~Parameters();

	// Get the parameter name at index pi
	std::string get_name(ParameterIndex pi) const;

	// Get the parameter value string at index pi
	std::string get_value_str(ParameterIndex pi) const;

	// Get (resp. set) non normalized value at parameter index pi
	float float_value(ParameterIndex pi) const;
	void set_value(ParameterIndex pi, float nf);

	// Get (resp. set) normalized [0, 1] value at parameter index pi
	float norm_float_value(ParameterIndex pi) const;
	void set_norm_value(ParameterIndex pi, float nf);

	// Convert parameters into string (useful for creating presets)
	std::string to_string(std::string indent=std::string()) const;

	Zynayumi& zynayumi;

	// Map parameter indices to Parameter
	std::vector<Parameter*> parameters;

	// Add parameters that are missing in Patch but need to be exposed
	// to the user
	float tone_detune;
	int tone_transpose;
	float ringmod_detune;
	int ringmod_transpose;
};

} // ~namespace zynayumi

#endif
