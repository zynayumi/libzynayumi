/****************************************************************************
    
    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    engine.hpp

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

#ifndef __ZYNAYUMI_ENGINE_HPP
#define __ZYNAYUMI_ENGINE_HPP

#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>

#include "voice.hpp"

extern "C"
{
#include "../../ayumi-lib/ayumi.h"
}

namespace zynayumi {

class Zynayumi;

/**
 * The engine holds the information of each voice, state of the
 * frequency, volume, sample offset, etc. And provide the render of
 * each voice and the whole mix.
 *
 * For the moment channels are ignored. Basically information from all
 * channels are all piped into the engine as if it were the same
 * channel.
 *
 * Eventual we probably want to have each voice associated to each
 * channel.
 */

class Engine {
public:

	///////////////////
	// Attributes    //
	///////////////////

	// Current ayumi state
	ayumi ay;

	// Current emulation mode (YM2149 or AY8910)
	EmulMode emulmode;

	// Current play mode
	PlayMode playmode;

	// Current pitches. Useful for handling chord based arp.
	std::multiset<unsigned char> pitches;

	// Stack of pitches, for mono mode
	std::vector<unsigned char> pitch_stack;

	// Keep track of the previous pitch for portamento. Negative means
	// none.
	double previous_pitch;
	double last_pitch;

	double lower_note_freq;
	double lower_note_freq_ym;
	int clock_rate;
	int sample_rate;             // Host sample rate
	double bpm;                  // Host beats per minute

	// Modulation wheel depth (in semitone)
	double mw_depth;
	
	// Pitch wheel pitch
	double pw_pitch;

	/////////////////////////////////
	// Constructors/descructors    //
	/////////////////////////////////

	Engine(const Zynayumi& ref);

	////////////////
	// Methods    //
	////////////////

	// Set sample rate
	void set_sample_rate(int sr);

	// Set bpm
	void set_bpm(double bpm);

	// Assumptions:
	//
	// 1. The parameters do not change during audio processing
	//
	// 2. All processing is added to the buffers
	void audio_process(float* left_out, float* right_out,
	                   unsigned long sample_count);

	void noteOn_process(unsigned char channel,
	                    unsigned char pitch,
	                    unsigned char velocity);

	void noteOff_process(unsigned char channel, unsigned char pitch);

	void allNotesOff_process();

	void modulation_process(unsigned char channel, unsigned char value);

	void pitchWheel_process(unsigned char channel, short value);

	void print(int m) const;

	double pitch2period_ym(double pitch) const;
	double freq2pitch(double freq) const;
	double smp2sec(unsigned long long smp_count) const;

private:
	int select_ym_channel() const;
	void set_last_pitch(unsigned char pitch);
	void add_voice(unsigned char pitch, unsigned char velocity);
	void add_all_voices(unsigned char pitch, unsigned char velocity);
	void free_voice();
	void free_all_voices();
	void set_all_voices_with_pitch(unsigned char pitch);
	void set_note_off_with_pitch(unsigned char pitch);
	void set_note_off_on_all_voices();

	const Zynayumi& _zynayumi;

	// Map pitch (possibly several times the same) to a voice
	typedef std::vector<Voice> Voices;
	Voices _voices;

	const unsigned char _max_voices;
};

} // ~namespace zynayumi

#endif
