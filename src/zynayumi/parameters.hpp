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
	return zynayumi::to_string(*value_ptr); // defined in patch.{hpp,cpp}
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
	TONE_SYNC,
	TONE_PHASE,
	TONE_TIME,
	TONE_DETUNE,
	TONE_TRANSPOSE,
	TONE_SPREAD,
	TONE_LEGACY_TUNING,

	// Noise
	NOISE_TIME,
	NOISE_PERIOD,

	// Noise Period Envelope
	NOISE_PERIOD_ENV_ATTACK,
	NOISE_PERIOD_ENV_TIME,

	// Envelope
	ENV_ATTACK_TIME,
	ENV_HOLD1_LEVEL,             // NEXT: 16
	ENV_INTER1_TIME,
	ENV_HOLD2_LEVEL,
	ENV_INTER2_TIME,
	ENV_HOLD3_LEVEL,
	ENV_DECAY_TIME,
	ENV_SUSTAIN_LEVEL,
	ENV_RELEASE,

	// Pitch envelope
	PITCH_ENV_ATTACK_PITCH,
	PITCH_ENV_TIME,

	// Arpegio
	SEQ_TONE_PITCH_0,
	SEQ_NOISE_PERIOD_0,
	SEQ_RINGMOD_DEPTH_0,
	SEQ_LEVEL_0,
	SEQ_TONE_PITCH_1,
	SEQ_NOISE_PERIOD_1,
	SEQ_RINGMOD_DEPTH_1,
	SEQ_LEVEL_1,
	SEQ_TONE_PITCH_2,
	SEQ_NOISE_PERIOD_2,
	SEQ_RINGMOD_DEPTH_2,
	SEQ_LEVEL_2,
	SEQ_TONE_PITCH_3,
	SEQ_NOISE_PERIOD_3,
	SEQ_RINGMOD_DEPTH_3,
	SEQ_LEVEL_3,
	SEQ_TONE_PITCH_4,
	SEQ_NOISE_PERIOD_4,
	SEQ_RINGMOD_DEPTH_4,
	SEQ_LEVEL_4,
	SEQ_TONE_PITCH_5,
	SEQ_NOISE_PERIOD_5,
	SEQ_RINGMOD_DEPTH_5,
	SEQ_LEVEL_5,
	SEQ_TONE_PITCH_6,
	SEQ_NOISE_PERIOD_6,
	SEQ_RINGMOD_DEPTH_6,
	SEQ_LEVEL_6,
	SEQ_TONE_PITCH_7,
	SEQ_NOISE_PERIOD_7,
	SEQ_RINGMOD_DEPTH_7,
	SEQ_LEVEL_7,
	SEQ_TONE_PITCH_8,
	SEQ_NOISE_PERIOD_8,
	SEQ_RINGMOD_DEPTH_8,
	SEQ_LEVEL_8,
	SEQ_TONE_PITCH_9,
	SEQ_NOISE_PERIOD_9,
	SEQ_RINGMOD_DEPTH_9,
	SEQ_LEVEL_9,
	SEQ_TONE_PITCH_10,
	SEQ_NOISE_PERIOD_10,
	SEQ_RINGMOD_DEPTH_10,
	SEQ_LEVEL_10,
	SEQ_TONE_PITCH_11,
	SEQ_NOISE_PERIOD_11,
	SEQ_RINGMOD_DEPTH_11,
	SEQ_LEVEL_11,
	SEQ_TONE_PITCH_12,
	SEQ_NOISE_PERIOD_12,
	SEQ_RINGMOD_DEPTH_12,
	SEQ_LEVEL_12,
	SEQ_TONE_PITCH_13,
	SEQ_NOISE_PERIOD_13,
	SEQ_RINGMOD_DEPTH_13,
	SEQ_LEVEL_13,
	SEQ_TONE_PITCH_14,
	SEQ_NOISE_PERIOD_14,
	SEQ_RINGMOD_DEPTH_14,
	SEQ_LEVEL_14,
	SEQ_TONE_PITCH_15,
	SEQ_NOISE_PERIOD_15,
	SEQ_RINGMOD_DEPTH_15,
	SEQ_LEVEL_15,
	SEQ_TEMPO,
	SEQ_HOST_SYNC,
	SEQ_BEAT_DIVISOR,
	SEQ_BEAT_MULTIPLIER,
	SEQ_LOOP,
	SEQ_END,

	// Ring modulation
	RINGMOD_WAVEFORM_LEVEL1,     // NEXT 16
	RINGMOD_WAVEFORM_LEVEL2,
	RINGMOD_WAVEFORM_LEVEL3,
	RINGMOD_WAVEFORM_LEVEL4,
	RINGMOD_WAVEFORM_LEVEL5,
	RINGMOD_WAVEFORM_LEVEL6,
	RINGMOD_WAVEFORM_LEVEL7,
	RINGMOD_WAVEFORM_LEVEL8,
	RINGMOD_SYNC,
	RINGMOD_PHASE,
	RINGMOD_MIRROR,
	RINGMOD_DETUNE,
	RINGMOD_TRANSPOSE,
	RINGMOD_FIXED_FREQUENCY,
	RINGMOD_FIXED_VS_RELATIVE,
	RINGMOD_DEPTH,               // NEXT: 16?

	// Buzzer
	BUZZER_SHAPE,
	BUZZER_SYNC,
	BUZZER_PHASE,
	BUZZER_TIME,
	BUZZER_DETUNE,
	BUZZER_TRANSPOSE,

	// Pitch LFO
	LFO_SHAPE,
	LFO_FREQ,
	LFO_DELAY,
	LFO_DEPTH,

	// Portamento time
	PORTAMENTO_TIME,
	PORTAMENTO_SMOOTHNESS,

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
#define TONE_SYNC_NAME "Tone sync"
#define TONE_PHASE_NAME "Tone phase"
#define TONE_TIME_NAME "Tone time"
#define TONE_DETUNE_NAME "Tone detune"
#define TONE_TRANSPOSE_NAME "Tone transpose"
#define TONE_SPREAD_NAME "Tone spread"
#define TONE_LEGACY_TUNING_NAME "Tone legacy tuning"
#define NOISE_TIME_NAME "Noise time"
#define NOISE_PERIOD_NAME "Noise period"
#define NOISE_PERIOD_ENV_ATTACK_NAME "NoisePeriodEnv attack"
#define NOISE_PERIOD_ENV_TIME_NAME "NoisePeriodEnv time"
#define ENV_ATTACK_TIME_NAME "Env attack time"
#define ENV_HOLD1_LEVEL_NAME "Env hold level 1"
#define ENV_INTER1_TIME_NAME "Env inter time 1"
#define ENV_HOLD2_LEVEL_NAME "Env hold level 2"
#define ENV_INTER2_TIME_NAME "Env inter time 2"
#define ENV_HOLD3_LEVEL_NAME "Env hold level 3"
#define ENV_DECAY_TIME_NAME "Env decay time"
#define ENV_SUSTAIN_LEVEL_NAME "Env sustain level"
#define ENV_RELEASE_NAME "Env release"
#define PITCH_ENV_ATTACK_PITCH_NAME "Pitch env attack pitch"
#define PITCH_ENV_TIME_NAME "Pitch env time"
#define SEQ_TONE_PITCH_0_NAME "Seq tone pitch 0"
#define SEQ_NOISE_PERIOD_0_NAME "Seq noise period 0"
#define SEQ_RINGMOD_DEPTH_0_NAME "Seq ringmod depth 0"
#define SEQ_LEVEL_0_NAME "Seq level 0"
#define SEQ_TONE_PITCH_1_NAME "Seq tone pitch 1"
#define SEQ_NOISE_PERIOD_1_NAME "Seq noise period 1"
#define SEQ_RINGMOD_DEPTH_1_NAME "Seq ringmod depth 1"
#define SEQ_LEVEL_1_NAME "Seq level 1"
#define SEQ_TONE_PITCH_2_NAME "Seq tone pitch 2"
#define SEQ_NOISE_PERIOD_2_NAME "Seq noise period 2"
#define SEQ_RINGMOD_DEPTH_2_NAME "Seq ringmod depth 2"
#define SEQ_LEVEL_2_NAME "Seq level 2"
#define SEQ_TONE_PITCH_3_NAME "Seq tone pitch 3"
#define SEQ_NOISE_PERIOD_3_NAME "Seq noise period 3"
#define SEQ_RINGMOD_DEPTH_3_NAME "Seq ringmod depth 3"
#define SEQ_LEVEL_3_NAME "Seq level 3"
#define SEQ_TONE_PITCH_4_NAME "Seq tone pitch 4"
#define SEQ_NOISE_PERIOD_4_NAME "Seq noise period 4"
#define SEQ_RINGMOD_DEPTH_4_NAME "Seq ringmod depth 4"
#define SEQ_LEVEL_4_NAME "Seq level 4"
#define SEQ_TONE_PITCH_5_NAME "Seq tone pitch 5"
#define SEQ_NOISE_PERIOD_5_NAME "Seq noise period 5"
#define SEQ_RINGMOD_DEPTH_5_NAME "Seq ringmod depth 5"
#define SEQ_LEVEL_5_NAME "Seq level 5"
#define SEQ_TONE_PITCH_6_NAME "Seq tone pitch 6"
#define SEQ_NOISE_PERIOD_6_NAME "Seq noise period 6"
#define SEQ_RINGMOD_DEPTH_6_NAME "Seq ringmod depth 6"
#define SEQ_LEVEL_6_NAME "Seq level 6"
#define SEQ_TONE_PITCH_7_NAME "Seq tone pitch 7"
#define SEQ_NOISE_PERIOD_7_NAME "Seq noise period 7"
#define SEQ_RINGMOD_DEPTH_7_NAME "Seq ringmod depth 7"
#define SEQ_LEVEL_7_NAME "Seq level 7"
#define SEQ_TONE_PITCH_8_NAME "Seq tone pitch 8"
#define SEQ_NOISE_PERIOD_8_NAME "Seq noise period 8"
#define SEQ_RINGMOD_DEPTH_8_NAME "Seq ringmod depth 8"
#define SEQ_LEVEL_8_NAME "Seq level 8"
#define SEQ_TONE_PITCH_9_NAME "Seq tone pitch 9"
#define SEQ_NOISE_PERIOD_9_NAME "Seq noise period 9"
#define SEQ_RINGMOD_DEPTH_9_NAME "Seq ringmod depth 9"
#define SEQ_LEVEL_9_NAME "Seq level 9"
#define SEQ_TONE_PITCH_10_NAME "Seq tone pitch 10"
#define SEQ_NOISE_PERIOD_10_NAME "Seq noise period 10"
#define SEQ_RINGMOD_DEPTH_10_NAME "Seq ringmod depth 10"
#define SEQ_LEVEL_10_NAME "Seq level 10"
#define SEQ_TONE_PITCH_11_NAME "Seq tone pitch 11"
#define SEQ_NOISE_PERIOD_11_NAME "Seq noise period 11"
#define SEQ_RINGMOD_DEPTH_11_NAME "Seq ringmod depth 11"
#define SEQ_LEVEL_11_NAME "Seq level 11"
#define SEQ_TONE_PITCH_12_NAME "Seq tone pitch 12"
#define SEQ_NOISE_PERIOD_12_NAME "Seq noise period 12"
#define SEQ_RINGMOD_DEPTH_12_NAME "Seq ringmod depth 12"
#define SEQ_LEVEL_12_NAME "Seq level 12"
#define SEQ_TONE_PITCH_13_NAME "Seq tone pitch 13"
#define SEQ_NOISE_PERIOD_13_NAME "Seq noise period 13"
#define SEQ_RINGMOD_DEPTH_13_NAME "Seq ringmod depth 13"
#define SEQ_LEVEL_13_NAME "Seq level 13"
#define SEQ_TONE_PITCH_14_NAME "Seq tone pitch 14"
#define SEQ_NOISE_PERIOD_14_NAME "Seq noise period 14"
#define SEQ_RINGMOD_DEPTH_14_NAME "Seq ringmod depth 14"
#define SEQ_LEVEL_14_NAME "Seq level 14"
#define SEQ_TONE_PITCH_15_NAME "Seq tone pitch 15"
#define SEQ_NOISE_PERIOD_15_NAME "Seq noise period 15"
#define SEQ_RINGMOD_DEPTH_15_NAME "Seq ringmod depth 15"
#define SEQ_LEVEL_15_NAME "Seq level 15"
#define SEQ_TEMPO_NAME "Seq tempo"
#define SEQ_HOST_SYNC_NAME "Seq host sync"
#define SEQ_BEAT_DIVISOR_NAME "Seq beat divisor"
#define SEQ_BEAT_MULTIPLIER_NAME "Seq beat multiplier"
#define SEQ_LOOP_NAME "Seq loop"
#define SEQ_END_NAME "Seq end"
#define RINGMOD_WAVEFORM_LEVEL1_NAME "RingMod waveform level 1"
#define RINGMOD_WAVEFORM_LEVEL2_NAME "RingMod waveform level 2"
#define RINGMOD_WAVEFORM_LEVEL3_NAME "RingMod waveform level 3"
#define RINGMOD_WAVEFORM_LEVEL4_NAME "RingMod waveform level 4"
#define RINGMOD_WAVEFORM_LEVEL5_NAME "RingMod waveform level 5"
#define RINGMOD_WAVEFORM_LEVEL6_NAME "RingMod waveform level 6"
#define RINGMOD_WAVEFORM_LEVEL7_NAME "RingMod waveform level 7"
#define RINGMOD_WAVEFORM_LEVEL8_NAME "RingMod waveform level 8"
#define RINGMOD_SYNC_NAME "RingMod sync"
#define RINGMOD_PHASE_NAME "RingMod phase"
#define RINGMOD_MIRROR_NAME "RingMod mirror"
#define RINGMOD_DETUNE_NAME "RingMod detune"
#define RINGMOD_TRANSPOSE_NAME "RingMod transpose"
#define RINGMOD_FIXED_FREQUENCY_NAME "RingMod fixed frequency"
#define RINGMOD_FIXED_VS_RELATIVE_NAME "RingMod fixed vs relative"
#define RINGMOD_DEPTH_NAME "RingMod depth"
#define BUZZER_SHAPE_NAME "Buzzer shape"
#define BUZZER_SYNC_NAME "Buzzer sync"
#define BUZZER_PHASE_NAME "Buzzer phase"
#define BUZZER_TIME_NAME "Buzzer time"
#define BUZZER_DETUNE_NAME "Buzzer detune"
#define BUZZER_TRANSPOSE_NAME "Buzzer transpose"
#define LFO_SHAPE_NAME "LFO shape"
#define LFO_FREQ_NAME "LFO freq"
#define LFO_DELAY_NAME "LFO delay"
#define LFO_DEPTH_NAME "LFO depth"
#define PORTAMENTO_TIME_NAME "Portamento time"
#define PORTAMENTO_SMOOTHNESS_NAME "Portamento smoothness"
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
#define TONE_SYNC_UNIT EMPTY
#define TONE_PHASE_UNIT EMPTY
#define TONE_TIME_UNIT SECOND
#define TONE_DETUNE_UNIT SEMITONE
#define TONE_TRANSPOSE_UNIT SEMITONE
#define TONE_SPREAD_UNIT SEMITONE
#define TONE_LEGACY_TUNING_UNIT EMPTY
#define NOISE_TIME_UNIT SECOND
#define NOISE_PERIOD_UNIT EMPTY
#define NOISE_PERIOD_ENV_ATTACK_UNIT EMPTY
#define NOISE_PERIOD_ENV_TIME_UNIT SECOND
#define ENV_ATTACK_TIME_UNIT SECOND
#define ENV_HOLD1_LEVEL_UNIT EMPTY
#define ENV_INTER1_TIME_UNIT SECOND
#define ENV_HOLD2_LEVEL_UNIT EMPTY
#define ENV_INTER2_TIME_UNIT SECOND
#define ENV_HOLD3_LEVEL_UNIT EMPTY
#define ENV_DECAY_TIME_UNIT SECOND
#define ENV_SUSTAIN_LEVEL_UNIT EMPTY
#define ENV_RELEASE_UNIT SECOND
#define PITCH_ENV_ATTACK_PITCH_UNIT SEMITONE
#define PITCH_ENV_TIME_UNIT SECOND
#define SEQ_TONE_PITCH_UNIT SEMITONE
#define SEQ_NOISE_PERIOD_UNIT EMPTY
#define SEQ_RINGMOD_DEPTH_UNIT EMPTY
#define SEQ_LEVEL_UNIT EMPTY
#define SEQ_TEMPO_UNIT BPM
#define SEQ_HOST_SYNC_UNIT EMPTY
#define SEQ_BEAT_DIVISOR_UNIT EMPTY
#define SEQ_BEAT_MULTIPLIER_UNIT EMPTY
#define SEQ_LOOP_UNIT EMPTY
#define SEQ_END_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL1_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL2_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL3_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL4_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL5_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL6_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL7_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL8_UNIT EMPTY
#define RINGMOD_SYNC_UNIT EMPTY
#define RINGMOD_PHASE_UNIT EMPTY
#define RINGMOD_MIRROR_UNIT EMPTY
#define RINGMOD_DETUNE_UNIT SEMITONE
#define RINGMOD_TRANSPOSE_UNIT SEMITONE
#define RINGMOD_FIXED_FREQUENCY_UNIT HERTZ
#define RINGMOD_FIXED_VS_RELATIVE_UNIT EMPTY
#define RINGMOD_DEPTH_UNIT EMPTY
#define BUZZER_SHAPE_UNIT EMPTY
#define BUZZER_SYNC_UNIT EMPTY
#define BUZZER_PHASE_UNIT EMPTY
#define BUZZER_TIME_UNIT SECOND
#define BUZZER_DETUNE_UNIT SEMITONE
#define BUZZER_TRANSPOSE_UNIT SEMITONE
#define LFO_SHAPE_UNIT EMPTY
#define LFO_FREQ_UNIT HERTZ
#define LFO_DELAY_UNIT SECOND
#define LFO_DEPTH_UNIT EMPTY
#define PORTAMENTO_TIME_UNIT SECOND
#define PORTAMENTO_SMOOTHNESS_UNIT EMPTY
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
#define TONE_SYNC_DFLT true
#define TONE_PHASE_DFLT 0.0
#define TONE_TIME_DFLT std::numeric_limits<float>::infinity()
#define TONE_TIME_DFLT_ALT -1.0 // In case infinity isn't supported
#define TONE_DETUNE_DFLT 0.0
#define TONE_TRANSPOSE_DFLT 0
#define TONE_SPREAD_DFLT 0.0
#define TONE_LEGACY_TUNING_DFLT false
#define NOISE_TIME_DFLT 0.0
#define NOISE_PERIOD_DFLT 1
#define NOISE_PERIOD_ENV_ATTACK_DFLT 1
#define NOISE_PERIOD_ENV_TIME_DFLT 0.0
#define ENV_ATTACK_TIME_DFLT 0.0
#define ENV_HOLD1_LEVEL_DFLT 1.0
#define ENV_INTER1_TIME_DFLT 0.0
#define ENV_HOLD2_LEVEL_DFLT 1.0
#define ENV_INTER2_TIME_DFLT 0.0
#define ENV_HOLD3_LEVEL_DFLT 1.0
#define ENV_DECAY_TIME_DFLT 0.0
#define ENV_SUSTAIN_LEVEL_DFLT 1.0
#define ENV_RELEASE_DFLT 0.0
#define PITCH_ENV_ATTACK_PITCH_DFLT 0.0
#define PITCH_ENV_TIME_DFLT 0.0
#define SEQ_TONE_PITCH_DFLT 0
#define SEQ_NOISE_PERIOD_DFLT 0
#define SEQ_RINGMOD_DEPTH_DFLT 1.0
#define SEQ_LEVEL_DFLT 15
#define SEQ_TEMPO_DFLT 120.0
#define SEQ_HOST_SYNC_DFLT true
#define SEQ_BEAT_DIVISOR_DFLT 9
#define SEQ_BEAT_MULTIPLIER_DFLT 1
#define SEQ_LOOP_DFLT 0
#define SEQ_END_DFLT 0
#define RINGMOD_WAVEFORM_LEVEL1_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL2_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL3_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL4_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL5_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL6_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL7_DFLT 1.0
#define RINGMOD_WAVEFORM_LEVEL8_DFLT 1.0
#define RINGMOD_SYNC_DFLT true
#define RINGMOD_PHASE_DFLT 0.0
#define RINGMOD_MIRROR_DFLT true
#define RINGMOD_DETUNE_DFLT 0.0
#define RINGMOD_TRANSPOSE_DFLT 0
#define RINGMOD_FIXED_FREQUENCY_DFLT 1.0
#define RINGMOD_FIXED_VS_RELATIVE_DFLT 1.0
#define RINGMOD_DEPTH_DFLT 1.0
#define BUZZER_SHAPE_DFLT Buzzer::Shape::DownSaw
#define BUZZER_SYNC_DFLT true
#define BUZZER_PHASE_DFLT 0.0
#define BUZZER_TIME_DFLT 0.0
#define BUZZER_DETUNE_DFLT 0.0
#define BUZZER_TRANSPOSE_DFLT 0
#define LFO_SHAPE_DFLT LFO::Shape::Sine
#define LFO_FREQ_DFLT 4.5
#define LFO_DELAY_DFLT 0.0
#define LFO_DEPTH_DFLT 0.0
#define PORTAMENTO_TIME_DFLT 0.0
#define PORTAMENTO_SMOOTHNESS_DFLT 0.5
#define GAIN_DFLT 1.0
#define PAN0_DFLT 0.5
#define PAN1_DFLT 0.25
#define PAN2_DFLT 0.75
#define PITCH_WHEEL_DFLT 2
#define VELOCITY_SENSITIVITY_DFLT 0.5
#define MODULATION_SENSITIVITY_DFLT 0.5f

// Parameter ranges
#define TONE_SYNC_L 0.0f
#define TONE_SYNC_U 1.0f
#define TONE_PHASE_L 0.0f
#define TONE_PHASE_U 1.0f
#define TONE_TIME_L 0.0f
#define TONE_TIME_L_ALT -1.0f   // In case infinity isn't supported
#define TONE_TIME_U std::numeric_limits<float>::infinity()
#define TONE_TIME_U_ALT 10.0f   // In case infinity isn't supported
#define TONE_DETUNE_L -0.5f
#define TONE_DETUNE_U 0.5f
#define TONE_TRANSPOSE_L -36
#define TONE_TRANSPOSE_U 36
#define TONE_SPREAD_L 0
#define TONE_SPREAD_U 0.5
#define TONE_LEGACY_TUNING_L 0
#define TONE_LEGACY_TUNING_U 1
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
#define ENV_ATTACK_TIME_L 0.0f
#define ENV_ATTACK_TIME_U 10.0f
#define ENV_HOLD1_LEVEL_L 0.0f
#define ENV_HOLD1_LEVEL_U 1.0f
#define ENV_INTER1_TIME_L 0.0f
#define ENV_INTER1_TIME_U 10.0f
#define ENV_HOLD2_LEVEL_L 0.0f
#define ENV_HOLD2_LEVEL_U 1.0f
#define ENV_INTER2_TIME_L 0.0f
#define ENV_INTER2_TIME_U 10.0f
#define ENV_HOLD3_LEVEL_L 0.0f
#define ENV_HOLD3_LEVEL_U 1.0f
#define ENV_DECAY_TIME_L 0.0f
#define ENV_DECAY_TIME_U 10.0f
#define ENV_SUSTAIN_LEVEL_L 0.0f
#define ENV_SUSTAIN_LEVEL_U 1.0f
#define ENV_RELEASE_L 0.0f
#define ENV_RELEASE_U 10.0f
#define PITCH_ENV_ATTACK_PITCH_L -96.0f
#define PITCH_ENV_ATTACK_PITCH_U 96.0f
#define PITCH_ENV_TIME_L 0.0f
#define PITCH_ENV_TIME_U 10.0f
#define SEQ_TONE_PITCH_L -48
#define SEQ_TONE_PITCH_U 48
#define SEQ_NOISE_PERIOD_L -32
#define SEQ_NOISE_PERIOD_U 32
#define SEQ_RINGMOD_DEPTH_L 0.0
#define SEQ_RINGMOD_DEPTH_U 1.0
#define SEQ_LEVEL_L 0
#define SEQ_LEVEL_U 15
#define SEQ_TEMPO_L 30.0
#define SEQ_TEMPO_U 300.0
#define SEQ_HOST_SYNC_L 0
#define SEQ_HOST_SYNC_U 1
#define SEQ_BEAT_DIVISOR_L 1
#define SEQ_BEAT_DIVISOR_U 64
#define SEQ_BEAT_MULTIPLIER_L 1
#define SEQ_BEAT_MULTIPLIER_U 64
#define SEQ_LOOP_L 0
#define SEQ_LOOP_U 15
#define SEQ_END_L 0
#define SEQ_END_U 16
#define RINGMOD_WAVEFORM_LEVEL1_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL1_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL2_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL2_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL3_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL3_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL4_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL4_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL5_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL5_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL6_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL6_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL7_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL7_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL8_L 0.0f
#define RINGMOD_WAVEFORM_LEVEL8_U 1.0f
#define RINGMOD_SYNC_L 0.0f
#define RINGMOD_SYNC_U 1.0f
#define RINGMOD_PHASE_L 0.0f
#define RINGMOD_PHASE_U 1.0f
#define RINGMOD_MIRROR_L 0.0f
#define RINGMOD_MIRROR_U 1.0f
#define RINGMOD_DETUNE_L -0.5f
#define RINGMOD_DETUNE_U 0.5f
#define RINGMOD_TRANSPOSE_L -36
#define RINGMOD_TRANSPOSE_U 36
#define RINGMOD_FIXED_FREQUENCY_L 1.0f
#define RINGMOD_FIXED_FREQUENCY_U 5000.0f
#define RINGMOD_FIXED_VS_RELATIVE_L 0.0f
#define RINGMOD_FIXED_VS_RELATIVE_U 1.0f
#define RINGMOD_DEPTH_L 0.0f
#define RINGMOD_DEPTH_U 1.0f
#define BUZZER_SYNC_L 0.0f
#define BUZZER_SYNC_U 1.0f
#define BUZZER_PHASE_L 0.0f
#define BUZZER_PHASE_U 1.0f
#define BUZZER_TIME_L 0.0f
#define BUZZER_TIME_L_ALT -1.0f   // In case infinity isn't supported
#define BUZZER_TIME_U std::numeric_limits<float>::infinity()
#define BUZZER_TIME_U_ALT 10.0f   // In case infinity isn't supported
#define BUZZER_DETUNE_L -0.5f
#define BUZZER_DETUNE_U 0.5f
#define BUZZER_TRANSPOSE_L -36
#define BUZZER_TRANSPOSE_U 36
#define LFO_FREQ_L 0.0f
#define LFO_FREQ_U 20.0f
#define LFO_DELAY_L 0.0f
#define LFO_DELAY_U 10.0f
#define LFO_DEPTH_L 0.0f
#define LFO_DEPTH_U 12.0f
#define PORTAMENTO_TIME_L 0.0f
#define PORTAMENTO_TIME_U 2.0f
#define PORTAMENTO_SMOOTHNESS_L 0.0f
#define PORTAMENTO_SMOOTHNESS_U 1.0f
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
	int seq_beat_divisor;
	int seq_beat_multiplier;
	float ringmod_detune;
	int ringmod_transpose;
	float buzzer_detune;
	int buzzer_transpose;
};

} // ~namespace zynayumi

#endif
