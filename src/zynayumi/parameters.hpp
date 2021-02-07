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

	// Lower/upper float bound
	virtual float float_low() const = 0;
	virtual float float_up() const = 0;

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

	// Lower/upper float bound
	float float_low() const override;
	float float_up() const override;

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

	// Lower/upper float bound
	float float_low() const override;
	float float_up() const override;

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

	// Lower/upper float bound
	float float_low() const override;
	float float_up() const override;

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
	                  float low, float up, bool percent=false);

	// Get/set methods
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Arc tangent range [atan_low, atan_up]
	float atan_low;
	float atan_up;

	// Whether the parameter should be expressed in percentage, in this
	// case the unit is %.  This is to avoid having inf (which is
	// possible with Tan mapping) involved in the range if the plugin
	// format does not support it.
	bool percent;
};

class BaseEnumParameter : public Parameter {
public:
	// Ctor
	BaseEnumParameter(const std::string& name, const std::string& unit, size_t cnt);

	// Lower/upper float bound
	float float_low() const override;
	float float_up() const override;

	// Return string representation of enum value
	virtual std::string enum_value_to_string(size_t ei) const = 0;

	// Number of enumerated values
	size_t count;
};

template<typename E>
class EnumParameter : public BaseEnumParameter {
public:
	// Ctor
	EnumParameter(const std::string& name, const std::string& unit,
	              E* value_ptr, E value_dflt);

	// Convert value parameter into string
	std::string value_to_string() const override;

	// Return string representation of enum value
	std::string enum_value_to_string(size_t ei) const override;

	// Get/set methods
	float float_value() const override;
	void set_value(float f) override;
	float norm_float_value() const override;
	void set_norm_value(float nf) override;

	// Overload assignment
	EnumParameter<E>& operator=(E e);

	// Current value
	E* value_ptr;
};

template<typename E>
EnumParameter<E>::EnumParameter(const std::string& n,
                                const std::string& u,
                                E* vl_ptr, E vl_dflt)
	: BaseEnumParameter(n, u, (size_t)E::Count), value_ptr(vl_ptr)
{
	*value_ptr = vl_dflt;
}

template<typename E>
std::string EnumParameter<E>::value_to_string() const
{
	return zynayumi::to_string(*value_ptr); // defined in patch.{hpp,cpp}
}

template<typename E>
std::string EnumParameter<E>::enum_value_to_string(size_t ei) const
{
	return zynayumi::to_string((E)ei); // defined in patch.{hpp,cpp}
}

template<typename E>
float EnumParameter<E>::float_value() const
{
	return (float)*value_ptr;
}

template<typename E>
void EnumParameter<E>::set_value(float nf)
{
	*value_ptr = (E)std::lround(nf);
}

template<typename E>
float EnumParameter<E>::norm_float_value() const
{
	return (float)*value_ptr / (float)((int)E::Count - 1);
}

template<typename E>
void EnumParameter<E>::set_norm_value(float nf)
{
	*value_ptr = (E)std::lround(nf * (float)((int)E::Count - 1));
}

template<typename E>
EnumParameter<E>& EnumParameter<E>::operator=(E e)
{
	*value_ptr = e;
	return *this;
}

// Parameter indices
enum ParameterIndex {
	// Emulation mode (YM2149 vs AY-3-8910)
	EMUL_MODE,

	// Play mode
	PLAY_MODE,

	// Tone
	TONE_RESET,
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
	ENV_HOLD1_LEVEL,
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
	PITCH_ENV_SMOOTHNESS,

	// Ring modulation
	RINGMOD_WAVEFORM_LEVEL1,
	RINGMOD_WAVEFORM_LEVEL2,
	RINGMOD_WAVEFORM_LEVEL3,
	RINGMOD_WAVEFORM_LEVEL4,
	RINGMOD_WAVEFORM_LEVEL5,
	RINGMOD_WAVEFORM_LEVEL6,
	RINGMOD_WAVEFORM_LEVEL7,
	RINGMOD_WAVEFORM_LEVEL8,
	RINGMOD_WAVEFORM_LEVEL9,
	RINGMOD_WAVEFORM_LEVEL10,
	RINGMOD_WAVEFORM_LEVEL11,
	RINGMOD_WAVEFORM_LEVEL12,
	RINGMOD_WAVEFORM_LEVEL13,
	RINGMOD_WAVEFORM_LEVEL14,
	RINGMOD_WAVEFORM_LEVEL15,
	RINGMOD_WAVEFORM_LEVEL16,
	RINGMOD_RESET,
	RINGMOD_SYNC,
	RINGMOD_PHASE,
	RINGMOD_LOOP,
	RINGMOD_DETUNE,
	RINGMOD_TRANSPOSE,
	RINGMOD_FIXED_PITCH,
	RINGMOD_FIXED_VS_RELATIVE,
	RINGMOD_DEPTH,

	// Buzzer
	BUZZER_ENABLED,
	BUZZER_SHAPE,

	// Sequencer
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
	RINGMOD_VELOCITY_SENSITIVITY,
	NOISE_PERIOD_PITCH_SENSITIVITY,
	MODULATION_SENSITIVITY,

	// Oversampling
	OVERSAMPLING,

	// Number of Parameters
	PARAMETERS_COUNT
};

// Parameter names
#define EMUL_MODE_NAME "Emulation mode"
#define PLAY_MODE_NAME "Play mode"
#define TONE_RESET_NAME "Tone reset"
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
#define PITCH_ENV_ATTACK_PITCH_NAME "PitchEnv attack pitch"
#define PITCH_ENV_TIME_NAME "PitchEnv time"
#define PITCH_ENV_SMOOTHNESS_NAME "PitchEnv smoothness"
#define RINGMOD_WAVEFORM_LEVEL_NAME "RingMod waveform level"
#define RINGMOD_RESET_NAME "RingMod reset"
#define RINGMOD_SYNC_NAME "RingMod sync"
#define RINGMOD_PHASE_NAME "RingMod phase"
#define RINGMOD_LOOP_NAME "RingMod loop"
#define RINGMOD_DETUNE_NAME "RingMod detune"
#define RINGMOD_TRANSPOSE_NAME "RingMod transpose"
#define RINGMOD_FIXED_PITCH_NAME "RingMod fixed pitch"
#define RINGMOD_FIXED_VS_RELATIVE_NAME "RingMod fixed vs relative"
#define RINGMOD_DEPTH_NAME "RingMod depth"
#define BUZZER_ENABLED_NAME "Buzzer enabled"
#define BUZZER_SHAPE_NAME "Buzzer shape"
#define SEQ_TONE_PITCH_NAME "Seq tone pitch"
#define SEQ_NOISE_PERIOD_NAME "Seq noise period"
#define SEQ_RINGMOD_DEPTH_NAME "Seq ringmod depth"
#define SEQ_LEVEL_NAME "Seq level"
#define SEQ_TEMPO_NAME "Seq tempo"
#define SEQ_HOST_SYNC_NAME "Seq host sync"
#define SEQ_BEAT_DIVISOR_NAME "Seq beat divisor"
#define SEQ_BEAT_MULTIPLIER_NAME "Seq beat multiplier"
#define SEQ_LOOP_NAME "Seq loop"
#define SEQ_END_NAME "Seq end"
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
#define RINGMOD_VELOCITY_SENSITIVITY_NAME "Ringmod velocity sensitivity"
#define NOISE_PERIOD_PITCH_SENSITIVITY_NAME "Noise period pitch sensitivity"
#define MODULATION_SENSITIVITY_NAME "Modulation sensitivity"
#define OVERSAMPLING_NAME "Oversampling"

// Parameter units
#define SECOND "sec"
#define SEMITONE "semitone"
#define HERTZ "Hz"
#define BPM "bpm"
#define EMPTY ""
#define EMUL_MODE_UNIT EMPTY
#define PLAY_MODE_UNIT EMPTY
#define TONE_RESET_UNIT EMPTY
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
#define PITCH_ENV_SMOOTHNESS_UNIT EMPTY
#define RINGMOD_WAVEFORM_LEVEL_UNIT EMPTY
#define RINGMOD_RESET_UNIT EMPTY
#define RINGMOD_SYNC_UNIT EMPTY
#define RINGMOD_PHASE_UNIT EMPTY
#define RINGMOD_LOOP_UNIT EMPTY
#define RINGMOD_DETUNE_UNIT SEMITONE
#define RINGMOD_TRANSPOSE_UNIT SEMITONE
#define RINGMOD_FIXED_PITCH_UNIT SEMITONE
#define RINGMOD_FIXED_VS_RELATIVE_UNIT EMPTY
#define RINGMOD_DEPTH_UNIT EMPTY
#define BUZZER_ENABLED_UNIT EMPTY
#define BUZZER_SHAPE_UNIT EMPTY
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
#define RINGMOD_VELOCITY_SENSITIVITY_UNIT EMPTY
#define NOISE_PERIOD_PITCH_SENSITIVITY_UNIT EMPTY
#define MODULATION_SENSITIVITY_UNIT EMPTY
#define OVERSAMPLING_UNIT EMPTY

// Parameter defaults
#define EMUL_MODE_DFLT EmulMode::YM2149
#define PLAY_MODE_DFLT PlayMode::Mono
#define TONE_RESET_DFLT true
#define TONE_PHASE_DFLT 0.0f
#define TONE_TIME_DFLT std::numeric_limits<float>::infinity()
#define TONE_TIME_DFLT_ALT -1.0f // In case infinity isn't supported
#define TONE_DETUNE_DFLT 0.0f
#define TONE_TRANSPOSE_DFLT 0
#define TONE_SPREAD_DFLT 0.0f
#define TONE_LEGACY_TUNING_DFLT false
#define NOISE_TIME_DFLT 0.0f
#define NOISE_PERIOD_DFLT 16
#define NOISE_PERIOD_ENV_ATTACK_DFLT 1
#define NOISE_PERIOD_ENV_TIME_DFLT 0.0f
#define ENV_ATTACK_TIME_DFLT 0.0f
#define ENV_HOLD1_LEVEL_DFLT MAX_LEVEL
#define ENV_INTER1_TIME_DFLT 0.0f
#define ENV_HOLD2_LEVEL_DFLT MAX_LEVEL
#define ENV_INTER2_TIME_DFLT 0.0f
#define ENV_HOLD3_LEVEL_DFLT MAX_LEVEL
#define ENV_DECAY_TIME_DFLT 0.0f
#define ENV_SUSTAIN_LEVEL_DFLT MAX_LEVEL
#define ENV_RELEASE_DFLT 0.0f
#define PITCH_ENV_ATTACK_PITCH_DFLT 0.0f
#define PITCH_ENV_TIME_DFLT 0.0f
#define PITCH_ENV_SMOOTHNESS_DFLT 0.5f
#define RINGMOD_WAVEFORM_LEVEL_DFLT MAX_LEVEL
#define RINGMOD_RESET_DFLT true
#define RINGMOD_SYNC_DFLT false
#define RINGMOD_PHASE_DFLT 0.0f
#define RINGMOD_LOOP_DFLT RingMod::Loop::PingPong
#define RINGMOD_DETUNE_DFLT 0.0f
#define RINGMOD_TRANSPOSE_DFLT 0
#define RINGMOD_FIXED_PITCH_DFLT 0.0f
#define RINGMOD_FIXED_VS_RELATIVE_DFLT 1.0f
#define RINGMOD_DEPTH_DFLT MAX_LEVEL
#define SEQ_TONE_PITCH_DFLT 0
#define SEQ_NOISE_PERIOD_DFLT 0
#define SEQ_RINGMOD_DEPTH_DFLT MAX_LEVEL
#define SEQ_LEVEL_DFLT MAX_LEVEL
#define SEQ_TEMPO_DFLT 120.0f
#define SEQ_HOST_SYNC_DFLT true
#define SEQ_BEAT_DIVISOR_DFLT 9
#define SEQ_BEAT_MULTIPLIER_DFLT 1
#define SEQ_LOOP_DFLT 0
#define SEQ_END_DFLT 0
#define BUZZER_ENABLED_DFLT false
#define BUZZER_SHAPE_DFLT Buzzer::Shape::DownSaw
#define LFO_SHAPE_DFLT LFO::Shape::Sine
#define LFO_FREQ_DFLT 4.5f
#define LFO_DELAY_DFLT 0.0f
#define LFO_DEPTH_DFLT 0.0f
#define PORTAMENTO_TIME_DFLT 0.0f
#define PORTAMENTO_SMOOTHNESS_DFLT 0.5f
#define GAIN_DFLT 1.0f
#define PAN0_DFLT 0.5f
#define PAN1_DFLT 0.25f
#define PAN2_DFLT 0.75f
#define PITCH_WHEEL_DFLT 2
#define VELOCITY_SENSITIVITY_DFLT 0.5f
#define RINGMOD_VELOCITY_SENSITIVITY_DFLT 0.0f
#define NOISE_PERIOD_PITCH_SENSITIVITY_DFLT 0.0f
#define MODULATION_SENSITIVITY_DFLT 0.5f
#define OVERSAMPLING_DFLT 2

// Parameter ranges
#define TONE_RESET_L 0.0f
#define TONE_RESET_U 1.0f
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
#define ENV_HOLD1_LEVEL_L 0
#define ENV_HOLD1_LEVEL_U MAX_LEVEL
#define ENV_INTER1_TIME_L 0.0f
#define ENV_INTER1_TIME_U 10.0f
#define ENV_HOLD2_LEVEL_L 0
#define ENV_HOLD2_LEVEL_U MAX_LEVEL
#define ENV_INTER2_TIME_L 0.0f
#define ENV_INTER2_TIME_U 10.0f
#define ENV_HOLD3_LEVEL_L 0
#define ENV_HOLD3_LEVEL_U MAX_LEVEL
#define ENV_DECAY_TIME_L 0.0f
#define ENV_DECAY_TIME_U 10.0f
#define ENV_SUSTAIN_LEVEL_L 0
#define ENV_SUSTAIN_LEVEL_U MAX_LEVEL
#define ENV_RELEASE_L 0.0f
#define ENV_RELEASE_U 10.0f
#define PITCH_ENV_ATTACK_PITCH_L -96.0f
#define PITCH_ENV_ATTACK_PITCH_U 96.0f
#define PITCH_ENV_TIME_L 0.0f
#define PITCH_ENV_TIME_U 10.0f
#define PITCH_ENV_SMOOTHNESS_L 0.0f
#define PITCH_ENV_SMOOTHNESS_U 1.0f
#define RINGMOD_WAVEFORM_LEVEL_L 0
#define RINGMOD_WAVEFORM_LEVEL_U MAX_LEVEL
#define RINGMOD_RESET_L 0.0f
#define RINGMOD_RESET_U 1.0f
#define RINGMOD_SYNC_L 0.0f
#define RINGMOD_SYNC_U 1.0f
#define RINGMOD_PHASE_L 0.0f
#define RINGMOD_PHASE_U 1.0f
#define RINGMOD_DETUNE_L -0.5f
#define RINGMOD_DETUNE_U 0.5f
#define RINGMOD_TRANSPOSE_L -36
#define RINGMOD_TRANSPOSE_U 36
#define RINGMOD_FIXED_PITCH_L 0.0f
#define RINGMOD_FIXED_PITCH_U 127.0f
#define RINGMOD_FIXED_VS_RELATIVE_L 0.0f
#define RINGMOD_FIXED_VS_RELATIVE_U 1.0f
#define RINGMOD_DEPTH_L 0
#define RINGMOD_DEPTH_U MAX_LEVEL
#define BUZZER_ENABLED_L 0.0f
#define BUZZER_ENABLED_U 1.0f
#define SEQ_TONE_PITCH_L -48
#define SEQ_TONE_PITCH_U 48
#define SEQ_NOISE_PERIOD_L -15
#define SEQ_NOISE_PERIOD_U 15
#define SEQ_RINGMOD_DEPTH_L 0
#define SEQ_RINGMOD_DEPTH_U MAX_LEVEL
#define SEQ_LEVEL_L 0
#define SEQ_LEVEL_U MAX_LEVEL
#define SEQ_TEMPO_L 30.0
#define SEQ_TEMPO_U 300.0
#define SEQ_HOST_SYNC_L 0
#define SEQ_HOST_SYNC_U 1
#define SEQ_BEAT_DIVISOR_L 1
#define SEQ_BEAT_DIVISOR_U 64
#define SEQ_BEAT_MULTIPLIER_L 1
#define SEQ_BEAT_MULTIPLIER_U 64
#define SEQ_LOOP_L 0
#define SEQ_LOOP_U 16
#define SEQ_END_L 0
#define SEQ_END_U 16
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
#define RINGMOD_VELOCITY_SENSITIVITY_L 0.0f
#define RINGMOD_VELOCITY_SENSITIVITY_U 1.0f
#define NOISE_PERIOD_PITCH_SENSITIVITY_L 0.0f
#define NOISE_PERIOD_PITCH_SENSITIVITY_U 1.0f
#define MODULATION_SENSITIVITY_L 0.0f
#define MODULATION_SENSITIVITY_U 12.0f
#define OVERSAMPLING_L 1
#define OVERSAMPLING_U 4

class Zynayumi;

class Parameters {
public:
	// CTor, DTor
	Parameters(Zynayumi& zynayumi, Patch& patch);
	~Parameters();

	Parameters& operator=(const Parameters& other);

	// Get the parameter name at index pi
	std::string get_name(ParameterIndex pi) const;

	// Get the parameter symbol at index pi (the name in lower case
	// and without space)
	std::string get_symbol(ParameterIndex pi) const;

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

	// Get the non normalized lower/upper bound in float
	float float_low(ParameterIndex pi) const;
	float float_up(ParameterIndex pi) const;

	// If the parameter is int/enum
	bool is_int(ParameterIndex pi) const;
	bool is_enum(ParameterIndex pi) const;
	bool is_percent(ParameterIndex pi) const;

	// If the parameter is an enum then return its count
	size_t enum_count(ParameterIndex pi) const;

	// If the parameter is an enum then return the name of its i_th
	// element
	std::string enum_value_name(ParameterIndex pi, size_t ei) const;

	// Call after setting changing a value
	void update(ParameterIndex pi);
	void update();

	// Convert parameters into string (useful for creating presets)
	std::string to_string(std::string indent=std::string()) const;

	Zynayumi& zynayumi;
	Patch& patch;

	// Map parameter indices to Parameter
	std::vector<Parameter*> parameters;

	// Add parameters that are missing in Patch but need to be exposed
	// to the user
	float tone_detune;
	int tone_transpose;
	float ringmod_detune;
	int ringmod_transpose;
	int seq_beat_divisor;
	int seq_beat_multiplier;
};

} // ~namespace zynayumi

#endif
