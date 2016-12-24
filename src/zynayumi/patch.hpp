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

/**
 * Amplitude envelope. Durations are in second and levels are between
 * 0.0 and 1.0.
 */
class Env {
public:
	Env();

	float level1;               // Attack level
	float time1;                // Duration between attack and hold-1
	float level2;               // Hold-1 level
	float time2;                // Duration between hold-1 and hold-2
	float level3;               // Hold-2 level
	float time3;                // Duration between hold-2 and sustain
	float level4;               // Sustain level
	float release;              // Release
};

class Noise {
public:
	Noise();

	float time;                 // Noise duration in second, inf if negative
	float freq;                 // Noise frequency
};

class Arp {
public:
	Arp();

	float pitch1;               // First pitch in semi-tones
	float time1;                // First pitch duration in second
	float pitch2;               // Second pitch in semi-tones
	float time2;                // Second pitch duration in second
	float pitch3;               // Third pitch in semi-tones
	float time3;                // Third pitch duration in second
	bool repeat;                // Whether it should be repeated
};

/**
 * Buzz trick based on YM2149 envelope. The Buzz period is distinct
 * from the tone period and modulates its timbre. It is actually a
 * sync buzz because the envelope is reset at each tone period.
 */
class Buzz {
public:
	Buzz();

	int period;                 // Duration of the envelope in sample
	int shape;                  // Envelope shape
	float detune;               // Relative detune in semitone
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

	Env env;                    // Amplitude envelope
	Noise noise;                // Noise control
	Arp arp;                    // Arpeggio
	Buzz buzz;                  // Buzz
	LFO lfo;                    // LFO
	float port;                 // Portamento time in second
	float detune;               // Detune in semitone
};

} // ~namespace zynayumi

#endif
