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
	Parameter(const std::string& name, const std::string& unit);

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

	// Unit exposed to the user
	std::string unit;
};

class BoolParameter : public Parameter {
public:
	// Ctor
	BoolParameter(const std::string& name, const std::string& unit,
	              bool* value_ptr, bool value_dflt);

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
	IntParameter(const std::string& name, const std::string& unit,
	             int* value_ptr, int value_dflt,
	             int low, int up);

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
	FloatParameter(const std::string& name, const std::string& unit,
	               float* value_ptr, float value_dflt,
	               float low, float up);

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
	LinearFloatParameter(const std::string& name, const std::string& unit,
	                     float* value_ptr, float value_dflt,
	                     float low, float up);

	// Get/set methods
	float norm_float_value() const override;
	void set_norm_value(float nf) override;
};

class CubeFloatParameter : public FloatParameter {
public:
	// Ctor
	CubeFloatParameter(const std::string& name, const std::string& unit,
	                   float* value_ptr, float value_dflt,
	                   float low, float up);

	// Get/set methods
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Cubic root range [cbrt_low, cbrt_up]
	float cbrt_low;
	float cbrt_up;
};

class TanFloatParameter : public FloatParameter {
public:
	// Ctor
	TanFloatParameter(const std::string& name, const std::string& unit,
	                  float* value_ptr, float value_dflt,
	                  float low, float up);

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
	EnumParameter(const std::string& name, const std::string& unit,
	              E* value_ptr, E value_dflt);

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
EnumParameter<E>::EnumParameter(const std::string& n,
                                const std::string& u,
                                E* vl_ptr, E vl_dflt)
	: Parameter(n, u), value_ptr(vl_ptr)
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
	// Emulation mode (YM2149 vs AY-3-8910)
	EMUL_MODE,

	// Play mode
	PLAY_MODE,

	// Tone
	TONE_TIME,
	TONE_DETUNE,
	TONE_TRANSPOSE,
	TONE_SPREAD,

	// Noise
	NOISE_TIME,
	NOISE_PERIOD,

	// Noise Period Envelope
	NOISE_PERIOD_ENV_ATTACK,
	NOISE_PERIOD_ENV_TIME,

	// Amplitude envelope
	AMP_ENV_ATTACK_TIME,
	AMP_ENV_HOLD1_LEVEL,
	AMP_ENV_INTER1_TIME,
	AMP_ENV_HOLD2_LEVEL,
	AMP_ENV_INTER2_TIME,
	AMP_ENV_HOLD3_LEVEL,
	AMP_ENV_DECAY_TIME,
	AMP_ENV_SUSTAIN_LEVEL,
	AMP_ENV_RELEASE,

	// Pitch envelope
	PITCH_ENV_ATTACK_PITCH,
	PITCH_ENV_TIME,

	// Arpegio
	ARP_PITCH1,
	ARP_PITCH2,
	ARP_PITCH3,
	ARP_TEMPO,
	ARP_HOST_SYNC,
	ARP_BEAT_DIVISOR,
	ARP_BEAT_MULTIPLIER,
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
	RING_MOD_FIXED_FREQUENCY,
	RING_MOD_FIXED_VS_RELATIVE,

	// Pitch LFO
	LFO_FREQ,
	LFO_DELAY,
	LFO_DEPTH,

	// Portamento time
	PORTAMENTO_TIME,

	// Gain
	GAIN,
	
	// Pan
	PAN0,
	PAN1,
	PAN2,

	// Control
	PITCH_WHEEL,
	VELOCITY_SENSITIVITY,
	MODULATION_SENSITIVITY,

	// Number of Parameters
	PARAMETERS_COUNT
};

// Parameter names
#define EMUL_MODE_NAME "Emulation mode"
#define PLAY_MODE_NAME "Play mode"
#define TONE_TIME_NAME "Tone time"
#define TONE_DETUNE_NAME "Tone detune"
#define TONE_TRANSPOSE_NAME "Tone transpose"
#define TONE_SPREAD_NAME "Tone spread"
#define NOISE_TIME_NAME "Noise time"
#define NOISE_PERIOD_NAME "Noise period"
#define NOISE_PERIOD_ENV_ATTACK_NAME "NoisePeriodEnv attack"
#define NOISE_PERIOD_ENV_TIME_NAME "NoisePeriodEnv time"
#define AMP_ENV_ATTACK_TIME_NAME "AmpEnv attack time"
#define AMP_ENV_HOLD1_LEVEL_NAME "AmpEnv hold level 1"
#define AMP_ENV_INTER1_TIME_NAME "AmpEnv inter time 1"
#define AMP_ENV_HOLD2_LEVEL_NAME "AmpEnv hold level 2"
#define AMP_ENV_INTER2_TIME_NAME "AmpEnv inter time 2"
#define AMP_ENV_HOLD3_LEVEL_NAME "AmpEnv hold level 3"
#define AMP_ENV_DECAY_TIME_NAME "AmpEnv decay time"
#define AMP_ENV_SUSTAIN_LEVEL_NAME "AmpEnv sustain level"
#define AMP_ENV_RELEASE_NAME "AmpEnv release"
#define PITCH_ENV_ATTACK_PITCH_NAME "PitchEnv attack pitch"
#define PITCH_ENV_TIME_NAME "PitchEnv time"
#define ARP_PITCH1_NAME "Arp pitch 1"
#define ARP_PITCH2_NAME "Arp pitch 2"
#define ARP_PITCH3_NAME "Arp pitch 3"
#define ARP_TEMPO_NAME "Arp tempo"
#define ARP_HOST_SYNC_NAME "Arp host sync"
#define ARP_BEAT_DIVISOR_NAME "Arp beat divisor"
#define ARP_BEAT_MULTIPLIER_NAME "Arp beat multiplier"
#define ARP_REPEAT_NAME "Arp repeat"
#define RING_MOD_WAVEFORM_LEVEL1_NAME "RingMod waveform level 1"
#define RING_MOD_WAVEFORM_LEVEL2_NAME "RingMod waveform level 2"
#define RING_MOD_WAVEFORM_LEVEL3_NAME "RingMod waveform level 3"
#define RING_MOD_WAVEFORM_LEVEL4_NAME "RingMod waveform level 4"
#define RING_MOD_WAVEFORM_LEVEL5_NAME "RingMod waveform level 5"
#define RING_MOD_WAVEFORM_LEVEL6_NAME "RingMod waveform level 6"
#define RING_MOD_WAVEFORM_LEVEL7_NAME "RingMod waveform level 7"
#define RING_MOD_WAVEFORM_LEVEL8_NAME "RingMod waveform level 8"
#define RING_MOD_MIRROR_NAME "RingMod mirror"
#define RING_MOD_SYNC_NAME "RingMod sync"
#define RING_MOD_DETUNE_NAME "RingMod detune"
#define RING_MOD_TRANSPOSE_NAME "RingMod transpose"
#define RING_MOD_FIXED_FREQUENCY_NAME "RingMod fixed frequency"
#define RING_MOD_FIXED_VS_RELATIVE_NAME "RingMod fixed vs relative"
#define LFO_FREQ_NAME "LFO freq"
#define LFO_DELAY_NAME "LFO delay"
#define LFO_DEPTH_NAME "LFO depth"
#define PORTAMENTO_TIME_NAME "Portamento time"
#define GAIN_NAME "Gain"
#define PAN0_NAME "Pan 1"
#define PAN1_NAME "Pan 2"
#define PAN2_NAME "Pan 3"
#define PITCH_WHEEL_NAME "Pitch wheel"
#define VELOCITY_SENSITIVITY_NAME "Velocity sensitivity"
#define MODULATION_SENSITIVITY_NAME "Modulation sensitivity"

// Parameter units
#define SECOND "sec"
#define SEMITONE "semitone"
#define HERTZ "Hz"
#define BPM "bpm"
#define EMPTY ""
#define EMUL_MODE_UNIT EMPTY
#define PLAY_MODE_UNIT EMPTY
#define TONE_TIME_UNIT SECOND
#define TONE_DETUNE_UNIT SEMITONE
#define TONE_TRANSPOSE_UNIT SEMITONE
#define TONE_SPREAD_UNIT SEMITONE
#define NOISE_TIME_UNIT SECOND
#define NOISE_PERIOD_UNIT EMPTY
#define NOISE_PERIOD_ENV_ATTACK_UNIT EMPTY
#define NOISE_PERIOD_ENV_TIME_UNIT SECOND
#define AMP_ENV_ATTACK_TIME_UNIT SECOND
#define AMP_ENV_HOLD1_LEVEL_UNIT EMPTY
#define AMP_ENV_INTER1_TIME_UNIT SECOND
#define AMP_ENV_HOLD2_LEVEL_UNIT EMPTY
#define AMP_ENV_INTER2_TIME_UNIT SECOND
#define AMP_ENV_HOLD3_LEVEL_UNIT EMPTY
#define AMP_ENV_DECAY_TIME_UNIT SECOND
#define AMP_ENV_SUSTAIN_LEVEL_UNIT EMPTY
#define AMP_ENV_RELEASE_UNIT SECOND
#define PITCH_ENV_ATTACK_PITCH_UNIT SEMITONE
#define PITCH_ENV_TIME_UNIT SECOND
#define ARP_PITCH1_UNIT SEMITONE
#define ARP_PITCH2_UNIT SEMITONE
#define ARP_PITCH3_UNIT SEMITONE
#define ARP_TEMPO_UNIT BPM
#define ARP_HOST_SYNC_UNIT EMPTY
#define ARP_BEAT_DIVISOR_UNIT EMPTY
#define ARP_BEAT_MULTIPLIER_UNIT EMPTY
#define ARP_REPEAT_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL1_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL2_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL3_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL4_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL5_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL6_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL7_UNIT EMPTY
#define RING_MOD_WAVEFORM_LEVEL8_UNIT EMPTY
#define RING_MOD_MIRROR_UNIT EMPTY
#define RING_MOD_SYNC_UNIT EMPTY
#define RING_MOD_DETUNE_UNIT SEMITONE
#define RING_MOD_TRANSPOSE_UNIT SEMITONE
#define RING_MOD_FIXED_FREQUENCY_UNIT HERTZ
#define RING_MOD_FIXED_VS_RELATIVE_UNIT EMPTY
#define LFO_FREQ_UNIT HERTZ
#define LFO_DELAY_UNIT SECOND
#define LFO_DEPTH_UNIT EMPTY
#define PORTAMENTO_TIME_UNIT SECOND
#define GAIN_UNIT EMPTY
#define PAN0_UNIT EMPTY
#define PAN1_UNIT EMPTY
#define PAN2_UNIT EMPTY
#define PITCH_WHEEL_UNIT EMPTY
#define VELOCITY_SENSITIVITY_UNIT EMPTY
#define MODULATION_SENSITIVITY_UNIT EMPTY

// Parameter defaults
#define EMUL_MODE_DFLT EmulMode::YM2149
#define PLAY_MODE_DFLT PlayMode::Mono
#define TONE_TIME_DFLT std::numeric_limits<float>::infinity()
#define TONE_TIME_DFLT_ALT -1.0 // In case infinity isn't supported
#define TONE_DETUNE_DFLT 0.0
#define TONE_TRANSPOSE_DFLT 0
#define TONE_SPREAD_DFLT 0.0
#define NOISE_TIME_DFLT 0.0
#define NOISE_PERIOD_DFLT 1
#define NOISE_PERIOD_ENV_ATTACK_DFLT 1
#define NOISE_PERIOD_ENV_TIME_DFLT 0.0
#define AMP_ENV_ATTACK_TIME_DFLT 0.0
#define AMP_ENV_HOLD1_LEVEL_DFLT 1.0
#define AMP_ENV_INTER1_TIME_DFLT 0.0
#define AMP_ENV_HOLD2_LEVEL_DFLT 1.0
#define AMP_ENV_INTER2_TIME_DFLT 0.0
#define AMP_ENV_HOLD3_LEVEL_DFLT 1.0
#define AMP_ENV_DECAY_TIME_DFLT 0.0
#define AMP_ENV_SUSTAIN_LEVEL_DFLT 1.0
#define AMP_ENV_RELEASE_DFLT 0.0
#define PITCH_ENV_ATTACK_PITCH_DFLT 0.0
#define PITCH_ENV_TIME_DFLT 0.0
#define ARP_PITCH1_DFLT 0
#define ARP_PITCH2_DFLT 0
#define ARP_PITCH3_DFLT 0
#define ARP_TEMPO_DFLT 120
#define ARP_HOST_SYNC_DFLT 1
#define ARP_BEAT_DIVISOR_DFLT 9
#define ARP_BEAT_MULTIPLIER_DFLT 1
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
#define RING_MOD_FIXED_FREQUENCY_DFLT 1.0
#define RING_MOD_FIXED_VS_RELATIVE_DFLT 1.0
#define LFO_FREQ_DFLT 4.5
#define LFO_DELAY_DFLT 0.0
#define LFO_DEPTH_DFLT 0.0
#define PORTAMENTO_TIME_DFLT 0.0
#define GAIN_DFLT 1.0
#define PAN0_DFLT 0.5
#define PAN1_DFLT 0.25
#define PAN2_DFLT 0.75
#define PITCH_WHEEL_DFLT 2
#define VELOCITY_SENSITIVITY_DFLT 0.5
#define MODULATION_SENSITIVITY_DFLT 0.5f

// Parameter ranges
#define TONE_TIME_L 0.0f
#define TONE_TIME_L_ALT -1.0f   // In case infinity isn't supported
#define TONE_TIME_U std::numeric_limits<float>::infinity()
#define TONE_TIME_U_ALT 10.0f   // In case infinity isn't supported
#define TONE_DETUNE_L -0.5f
#define TONE_DETUNE_U 0.5f
#define TONE_TRANSPOSE_L -24
#define TONE_TRANSPOSE_U 24
#define TONE_SPREAD_L 0
#define TONE_SPREAD_U 0.5
#define NOISE_TIME_L 0.0f
#define NOISE_TIME_L_ALT -1.0f  // In case infinity isn't supported
#define NOISE_TIME_U std::numeric_limits<float>::infinity()
#define NOISE_TIME_U_ALT 10.0f  // In case infinity isn't supported
#define NOISE_PERIOD_L 1
#define NOISE_PERIOD_U 31
#define NOISE_PERIOD_ENV_ATTACK_L 1
#define NOISE_PERIOD_ENV_ATTACK_U 31
#define NOISE_PERIOD_ENV_TIME_L 0.0f
#define NOISE_PERIOD_ENV_TIME_U 10.0f
#define AMP_ENV_ATTACK_TIME_L 0.0f
#define AMP_ENV_ATTACK_TIME_U 10.0f
#define AMP_ENV_HOLD1_LEVEL_L 0.0f
#define AMP_ENV_HOLD1_LEVEL_U 1.0f
#define AMP_ENV_INTER1_TIME_L 0.0f
#define AMP_ENV_INTER1_TIME_U 10.0f
#define AMP_ENV_HOLD2_LEVEL_L 0.0f
#define AMP_ENV_HOLD2_LEVEL_U 1.0f
#define AMP_ENV_INTER2_TIME_L 0.0f
#define AMP_ENV_INTER2_TIME_U 10.0f
#define AMP_ENV_HOLD3_LEVEL_L 0.0f
#define AMP_ENV_HOLD3_LEVEL_U 1.0f
#define AMP_ENV_DECAY_TIME_L 0.0f
#define AMP_ENV_DECAY_TIME_U 10.0f
#define AMP_ENV_SUSTAIN_LEVEL_L 0.0f
#define AMP_ENV_SUSTAIN_LEVEL_U 1.0f
#define AMP_ENV_RELEASE_L 0.0f
#define AMP_ENV_RELEASE_U 10.0f
#define PITCH_ENV_ATTACK_PITCH_L -96.0f
#define PITCH_ENV_ATTACK_PITCH_U 96.0f
#define PITCH_ENV_TIME_L 0.0f
#define PITCH_ENV_TIME_U 10.0f
#define ARP_PITCH1_L -48
#define ARP_PITCH1_U 48
#define ARP_PITCH2_L -48
#define ARP_PITCH2_U 48
#define ARP_PITCH3_L -48
#define ARP_PITCH3_U 48
#define ARP_TEMPO_L 30
#define ARP_TEMPO_U 300
#define ARP_HOST_SYNC_L 0
#define ARP_HOST_SYNC_U 1
#define ARP_BEAT_DIVISOR_L 1
#define ARP_BEAT_DIVISOR_U 64
#define ARP_BEAT_MULTIPLIER_L 1
#define ARP_BEAT_MULTIPLIER_U 64
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
#define RING_MOD_FIXED_FREQUENCY_L 1.0f
#define RING_MOD_FIXED_FREQUENCY_U 5000.0f
#define RING_MOD_FIXED_VS_RELATIVE_L 0.0f
#define RING_MOD_FIXED_VS_RELATIVE_U 1.0f
#define LFO_FREQ_L 0.0f
#define LFO_FREQ_U 20.0f
#define LFO_DELAY_L 0.0f
#define LFO_DELAY_U 10.0f
#define LFO_DEPTH_L 0.0f
#define LFO_DEPTH_U 12.0f
#define PORTAMENTO_TIME_L 0.0f
#define PORTAMENTO_TIME_U 5.0f
#define GAIN_L 0.0f
#define GAIN_U 2.0f
#define PAN0_L 0.0f
#define PAN0_U 1.0f
#define PAN1_L 0.0f
#define PAN1_U 1.0f
#define PAN2_L 0.0f
#define PAN2_U 1.0f
#define PITCH_WHEEL_L 1
#define PITCH_WHEEL_U 12
#define VELOCITY_SENSITIVITY_L 0.0f
#define VELOCITY_SENSITIVITY_U 1.0f
#define MODULATION_SENSITIVITY_L 0.0f
#define MODULATION_SENSITIVITY_U 12.0f

class Zynayumi;

class Parameters {
public:
	// CTor, DTor
	Parameters(Zynayumi& zynayumi);
	~Parameters();

	// Get the parameter name at index pi
	std::string get_name(ParameterIndex pi) const;

	// Return a string of the unit: sec, dB, etc.
	std::string get_unit(ParameterIndex pi) const;

	// Get the parameter value string at index pi
	std::string get_value_str(ParameterIndex pi) const;

	// Get (resp. set) non normalized value at parameter index pi
	float float_value(ParameterIndex pi) const;
	void set_value(ParameterIndex pi, float nf);

	// Get (resp. set) normalized [0, 1] value at parameter index pi
	float norm_float_value(ParameterIndex pi) const;
	void set_norm_value(ParameterIndex pi, float nf);

	// Call after setting changing a value
	void update(ParameterIndex pi);

	// Convert parameters into string (useful for creating presets)
	std::string to_string(std::string indent=std::string()) const;

	Zynayumi& zynayumi;

	// Map parameter indices to Parameter
	std::vector<Parameter*> parameters;

	// Add parameters that are missing in Patch but need to be exposed
	// to the user
	float tone_detune;
	int tone_transpose;
	int arp_beat_divisor;
	int arp_beat_multiplier;
	float ringmod_detune;
	int ringmod_transpose;
};

} // ~namespace zynayumi

#endif
