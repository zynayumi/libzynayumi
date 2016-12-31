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

namespace zynayumi {

enum class PlayMode {
	Legato,
	UpArp,
	DownArp,
	RndArp                      // Not implemented
};

class Tone {
public:
	Tone();

	double time;                 // Tone duration in second, inf if negative
	double detune;               // Detune in semitone
};

class Noise {
public:
	Noise();

	double time;                 // Noise duration in second, inf if negative
	int period;                  // Noise period [0..31]
};

/**
 * Amplitude envelope. Durations are in second and levels are between
 * 0.0 and 1.0.
 */
class AmpEnv {
public:
	AmpEnv();

	double attack_level;         // Attack level
	double time1;                // Duration between attack and hold-1
	double level1;               // Hold-1 level
	double time2;                // Duration between hold-1 and hold-2
	double level2;               // Hold-2 level
	double time3;                // Duration between hold-2 and sustain
	double sustain_level;        // Sustain level
	double release;              // Release
};


/**
 * Pitch envelope. Durations are in second and pitches are in
 * semitones.
 */
class PitchEnv {
public:
	PitchEnv();

	double attack_pitch;         // Relative pitch of the attack
	double time;                 // Duration to go from attack pitch
                                 // and tone pitch
};

class Arp {
public:
	Arp();

	double pitch1;               // First pitch in semitone
	double pitch2;               // Second pitch in semitone
	double pitch3;               // Third pitch in semitone
	double freq;                 // Pitch change frequency
	int repeat;                  // Repeat point
};

/**
 * Ring modulation to allow the YM2149 to play other waveforms. The
 * waveform is represented by 4 samples, i.e. volume levels from 0.0
 * to 1.0. The modulating waveform is actually 8 samples long with the
 * last 4 samples mirroring the first 4. We want that to have a smooth
 * dephasing effect and that way only 4 values are required to define
 * a waveform.
 */
#define RING_MOD_WAVEFORM_SIZE 4
class RingMod {
public:
	RingMod();

	double waveform[RING_MOD_WAVEFORM_SIZE]; // Sample volume levels
	double detune;                           // Relative detune in semitone
                                             // compared to the tone
};

/**
 * Pitch LFO
 */
class LFO {
public:
	LFO();

	double freq;                 // LFO frequency
	double delay;                // LFO progressive delay in second
	double depth;                // LFO depth in semitone
};

/**
 * Complete patch
 */
class Patch {
public:
	Patch();

	PlayMode playmode;          // Legato or arp
	Tone tone;                  // Tone control
	Noise noise;                // Noise control
	AmpEnv ampenv;              // Amplitude envelope
	PitchEnv pitchenv;          // Pitch envelope
	Arp arp;                    // Arpeggio
	RingMod ringmod;            // Ring modulation
	LFO lfo;                    // LFO
	double port;                // Portamento time in second
};

} // ~namespace zynayumi

#endif
