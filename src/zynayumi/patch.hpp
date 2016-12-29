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

	float time;                 // Tone duration in second, inf if negative
	float detune;               // Detune in semitone
};

class Noise {
public:
	Noise();

	float time;                 // Noise duration in second, inf if negative
	float freq;                 // Noise frequency
};

/**
 * Amplitude envelope. Durations are in second and levels are between
 * 0.0 and 1.0.
 */
class Env {
public:
	Env();

	float attack_level;         // Attack level
	float time1;                // Duration between attack and hold-1
	float level1;               // Hold-1 level
	float time2;                // Duration between hold-1 and hold-2
	float level2;               // Hold-2 level
	float time3;                // Duration between hold-2 and sustain
	float sustain_level;        // Sustain level
	float release;              // Release
};

class Arp {
public:
	Arp();

	float pitch1;               // First pitch in semitone
	float pitch2;               // Second pitch in semitone
	float pitch3;               // Third pitch in semitone
	float freq;                 // Pitch change frequency
	int repeat;                 // Repeat point
};

/**
 * Ring modulation to allow the YM2149 to play other waveforms. The
 * waveform is represented by 8 samples, i.e. volume levels from 0.0
 * to 1.0.
 */
#define RING_MOD_WAVEFORM_SIZE 8
class RingMod {
public:
	RingMod();

	float waveform[RING_MOD_WAVEFORM_SIZE]; // Sample volume levels
	float detune;                           // Relative detune in semitone
                                            // compared to the tone
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

/**
 * Complete patch
 */
class Patch {
public:
	Patch();

	PlayMode playmode;          // Legato or arp
	Tone tone;                  // Tone control
	Noise noise;                // Noise control
	Env env;                    // Amplitude envelope
	Arp arp;                    // Arpeggio
	RingMod ringmod;            // Ring modulation
	LFO lfo;                    // LFO
	float port;                 // Portamento time in second
};

} // ~namespace zynayumi

#endif
