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

namespace zynayumi {

enum class EmulMode {
	YM2149,
	AY8910,

	Count
};

enum class PlayMode {
	Mono,
	MonoUpArp,
	MonoDownArp,
	MonoRndArp,
	Poly,
	Unison,
	UnisonUpArp,
	UnisonDownArp,
	UnisonRndArp,

	Count
};

class Tone {
public:
	Tone();

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
 * Amplitude envelope. Durations are in second and levels are between
 * 0.0 and 1.0.
 */
class AmpEnv {
public:
	AmpEnv();

	float attack_time;           // Attack time
	float hold1_level;           // Hold-1 level
	float inter1_time;           // Duration between hold-1 and hold-2
	float hold2_level;           // Hold-2 level
	float inter2_time;           // Duration between hold-2 and hold-3
	float hold3_level;           // Hold-3 level
	float decay_time;            // Duration between hold-3 and sustain
	float sustain_level;         // Sustain level
	float release;               // Release
};

/**
 * Pitch envelope. Durations are in second and pitches are in
 * semitones.
 */
class PitchEnv {
public:
	PitchEnv();

	float attack_pitch;          // Relative pitch of the attack
	float time;                  // Duration to go from attack pitch to
	                             // tone pitch.
};

class Arp {
public:
	Arp();

	int pitch1;                  // First pitch in semitone
	int pitch2;                  // Second pitch in semitone
	int pitch3;                  // Third pitch in semitone
	float tempo;                 // Tempo used to calculate the frequency
	bool host_sync;              // Where the tempo is determined by the host
	float freq;                  // Pitch change frequency
	int repeat;                  // Repeat point
};

/**
 * Ring modulation to allow the YM2149 to play other waveforms. The
 * waveform is represented by 8 samples, i.e. volume levels from 0.0
 * to 1.0. If the minor flag is set to true, then the modulating
 * waveform length doubles, so is 16 samples long with the last 8
 * samples mirroring the first 8. That is to obtain a smooth dephasing
 * effect when the tone is enabled.
 */
#define RING_MOD_WAVEFORM_SIZE 8
class RingMod {
public:
	RingMod();

	float waveform[RING_MOD_WAVEFORM_SIZE];  // Sample volume levels

	bool mirror;                             // Add 8 mirroring samples

	bool sync;                               // Whether ring
	                                         // modulation is in sync
	                                         // with YM2149 oscilator

	float detune;                            // Relative detune in
	                                         // semitone compared to
	                                         // the tone

	float fixed_freq;                        // Fixed frequency

	float fixed_vs_relative;                 // How much fixed vs
                                            // relative (0.0 is all
                                            // fixed, 1.0 is all
                                            // relative)

	float depth;                             // Ring modulation
                                            // intensity
};

class Buzzer {
	enum class Shape {
		DownSaw,                  // 8
		DownTriangle,             // 10
		UpSaw,                    // 12
		UpTriangle,               // 14

		Count
	};

	Shape shape;                 // Buzzer shape
	float time;                  // Buzzer duration in second, inf if
                                // negative.
	float detune;                // Detune in semitone.
};

/**
 * Pitch LFO
 */
class LFO {
public:
	LFO();

	float freq;                 // LFO frequency
	float delay;                // LFO progressive delay in second
	float depth;                // LFO depth in semitone
};

class Pan {
public:
	Pan();

	float ym_channel[3];
};

/**
 * Pitchwheel, velocity and modulation sensitivity
 */
class Control {
public:
	Control();

	// Range in semitone of the pitch wheel
	int pitchwheel;

	// Key press velocity sensitivity, ranges in [0, 1]
	float velocity_sensitivity;

	// How the LFO is sensitive to modulation, in semitone, ranges in [0, 12]
	float modulation_sensitivity;
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
	AmpEnv ampenv;              // Amplitude envelope
	PitchEnv pitchenv;          // Pitch envelope
	Arp arp;                    // Arpeggio
	RingMod ringmod;            // Ring modulation
	LFO lfo;                    // LFO
	float port;                 // Portamento time in second per semitone
	float gain;                 // Output gain
	Pan pan;                    // Channels panning
	Control control;            // Pitchwheel, velocity sensitivity, etc
};

std::string to_string(PlayMode pm);
std::string to_string(EmulMode em);

} // ~namespace zynayumi

#endif
