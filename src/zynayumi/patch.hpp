/****************************************************************************

    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    patch.hpp

    Copyleft (c) 2016 Nil Geisweiller
 
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

#ifndef __ZYNAYUMI_PATCH_HPP
#define __ZYNAYUMI_PATCH_HPP

#include <string>
#include <vector>

namespace zynayumi {

static const int MAX_LEVEL = 15;

enum class EmulMode {
	YM2149,
	AY8910,

	Count
};

enum class PlayMode {
	Mono,
	MonoUpArp,
	MonoDownArp,
	MonoRandArp,
	Unison,
	UnisonUpArp,
	UnisonDownArp,
	UnisonRandArp,
	Poly,

	Count
};

class Tone {
public:
	Tone();

	bool reset;                   // Whether the tone phase is reset at
                                // each new on note.

	float phase;                 // Phase, from 0.0 to 1.0

	float time;                  // Tone duration in second, inf if
                                // negative.

	float detune;                // Detune in semitone.

	float spread;                // Spread of detune of second
                                // (negative) and third (positive)
                                // voice, in semitone.

	bool legacy_tuning;          // Whether the tuning is perfect or
                                // legacy.
};

class Noise {
public:
	Noise();

	float time;                  // Noise duration in second, inf if
                                // negative.
	int period;                  // Noise period [0..31]
};

class NoisePeriodEnv {
public:
	NoisePeriodEnv();

	int attack;                  // Absolute period of the attack.
	float time;                  // Duration to go from attack period
                                // to regular noise period.
};

/**
 * Amplitude Envelope. Durations are in second and levels are between
 * 0 and 15.
 */
class Env {
public:
	Env();

	float attack_time;           // Attack time
	int hold1_level;             // Hold-1 level (0-15)
	float inter1_time;           // Duration between hold-1 and hold-2
	int hold2_level;             // Hold-2 level (0-15)
	float inter2_time;           // Duration between hold-2 and hold-3
	int hold3_level;             // Hold-3 level (0-15)
	float decay_time;            // Duration between hold-3 and sustain
	int sustain_level;           // Sustain level (0-15)
	float release;               // Release
};

/**
 * Pitch envelope. Durations are in second and pitches are in
 * semitones.
 */
class PitchEnv {
public:
	PitchEnv();

	int attack_pitch;            // Relative pitch of the attack
	float time;                  // Duration to go from attack pitch to
	                             // tone pitch.
	float smoothness;            // Smoothly reaches the destination pitch
};

/**
 * Ring modulation to allow the YM2149 to play other waveforms. The
 * waveform is represented by 8 samples, i.e. volume levels from 0.0
 * to 1.0. If the minor flag is set to true, then the modulating
 * waveform length doubles, so is 16 samples long with the last 8
 * samples mirroring the first 8. That is to obtain a smooth dephasing
 * effect when the tone is enabled.
 */
#define RINGMOD_WAVEFORM_SIZE 16
class RingMod {
public:
	RingMod();

	enum class Loop {
		Off,
		Forward,
		PingPong,

		Count
	};

	int waveform[RINGMOD_WAVEFORM_SIZE];     // Sample volume levels

	bool reset;                              // Whether ring modulation
	                                         // is reset at each key
	                                         // stroke

	bool sync;                               // Whether it is synced
														  // with tone wave cycle

	float phase;                             // Initial phase, from 0.0
	                                         // to 1.0

	Loop loop;                               // Waveform (or buzzer)
	                                         // loop mode

	float detune;                            // Relative detune in
	                                         // semitone compared to
	                                         // the tone

	float fixed_pitch;                       // Fixed pitch

	float fixed_vs_relative;                 // How much fixed vs
                                            // relative (0.0 is all
                                            // fixed, 1.0 is all
                                            // relative)

	int depth;                               // Ring modulation
                                            // intensity
};

class Buzzer {
public:
	Buzzer();

	enum class Shape {
		DownSaw,                  // 8
		UpSaw,                    // 12

		Count
	};

	bool enabled;					  // Whether the buzzer is enabled. If
	                             // so it inherits all attributes of
	                             // the ring modulator (reset, sync,
	                             // detune, etc).

	Shape shape;                 // Buzzer shape
};

class Seq {
public:
	struct State {
		State();

		int tone_pitch;           // Relative tone pitch in semitone
		int noise_period;         // Relative noise period
		int ringmod_pitch;        // Relative ringmod pitch in semitone
		int ringmod_depth;        // Depth of the ring modulator
		int level;                // Voice level
		bool tone_on;             // Enable tone
		bool noise_on;            // Enable noise
	};

	enum class SeqMode {
		Off,
		Forward,
		Backward,
		PingPong,
		Random,

		Count
	};

	Seq();

	static const unsigned size = 16;
	std::vector<State> states;   // Array of sequencer states
	SeqMode seqmode;             // Sequencer mode
	float tempo;                 // Tempo used to calculate the frequency
	bool host_sync;              // Where the tempo is determined by the host
	float freq;                  // Pitch change frequency
	int loop;                    // Looping point
	int end;                     // End point
};

/**
 * Pitch LFO
 */
class LFO {
public:
	LFO();

	enum class Shape {
		Sine,
		Triangle,
		DownSaw,
		UpSaw,
		Square,
		Rand,

		Count
	};

	Shape shape;                // LFO shape
	float freq;                 // LFO frequency
	float delay;                // LFO progressive delay in second
	float depth;                // LFO depth in semitone
};

/**
 * Portamento
 */
class Portamento {
public:
	Portamento();

	float time;
	float smoothness;
};

class Mixer {
public:
	Mixer();

	// Whether each ym channel is disabled
	bool enabled[3];

	// Pan associated to each ym channel
	float pan[3];

	// Output gain
	float gain;
};

/**
 * Pitchwheel, velocity and modulation sensitivity
 */
class Control {
public:
	Control();

	enum class MidiChannel {
		Any,
		c1,
		c2,
		c3,
		c4,
		c5,
		c6,
		c7,
		c8,
		c9,
		c10,
		c11,
		c12,
		c13,
		c14,
		c15,
		c16,

		Count
	};

	// Range in semitone of the pitch wheel
	int pitchwheel;

	// Key press velocity sensitivity, ranges in [0, 1]
	float velocity_sensitivity;

	// Ringmod depth velocity sensitivity
	float ringmod_velocity_sensitivity;

	// Noise period pitch sensitivity
	float noise_period_pitch_sensitivity;

	// How the LFO is sensitive to modulation, in semitone, ranges in [0, 12]
	float modulation_sensitivity;

	// Input midi channel associated to each ym channel
	MidiChannel midi_ch[3];
};
	
/**
 * Complete patch
 */
class Patch {
public:
	Patch();

	std::string name;           // Name

	EmulMode emulmode;          // Emulation mode, YM2149 or AY-3-8910
	PlayMode playmode;          // Monophonic, polyphonic, unison or arp
	Tone tone;                  // Tone control
	Noise noise;                // Noise control
	NoisePeriodEnv noise_period_env; // Noise period envelope
	Env env;                    // Envelope
	PitchEnv pitchenv;          // Pitch envelope
	RingMod ringmod;            // Ring modulation
	Buzzer buzzer;              // Buzzer
	Seq seq;                    // Sequencer
	LFO lfo;                    // LFO
	Portamento portamento;      // Portamento
	Mixer mixer;                // YM Channels panning and such
	Control control;            // Pitchwheel, velocity sensitivity, etc
};

std::string to_string(PlayMode pm);
std::string to_string(EmulMode em);
std::string to_string(RingMod::Loop lp);
std::string to_string(Buzzer::Shape sh);
std::string to_string(LFO::Shape sh);
std::string to_string(Control::MidiChannel mc);

} // ~namespace zynayumi

#endif
