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

	ayumi ay;                  // current ayumi state

	// Current pitches. Useful for handling chord based arp.
	//
	// TODO: have a map from channel to this multiset
	std::multiset<unsigned char> pitches;

	// Keep track of the previous pitch for portamento. Negative means
	// none.
	double previous_pitch;
	double last_pitch;

	// Keep track of the ring modulation smp count, the number of
	// samples since last ring modulation waveform cycle start, to
	// control synchronization with the YM2149 oscillators. Map each
	// YM2149 channel to the ringmod smp count.
	double ringmod_smp_count[3];

	// Keep track of the ring modulation waveform index as well. This
	// may affect just one sample, but it produces an occasional click
	// if not taken care of.
	unsigned ringmod_waveform_index[3];

	const double lower_note_freq;
	const double lower_note_freq_ym;
	const int sample_rate; // TODO: should be provided by the host
	const int clock_rate;

	// Pitch wheel pitch
	double pw_pitch;

	/////////////////////////////////
	// Constructors/descructors    //
	/////////////////////////////////

	Engine(const Zynayumi& ref);

	////////////////
	// Methods    //
	////////////////

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

	void pitchWheel_process(unsigned char channel, short value);

	void print(int m) const;

	double pitch2period(double pitch) const;
	// Same as pitch2period but for YM2149. The reason it is different
	// has to do with ayumi emulation, don't understand it.
	double pitch2period_ym(double pitch) const;
	double smp2sec(unsigned long long smp_count) const;

private:

	int select_ym_channel() const;
	void add_voice(unsigned char pitch, unsigned char velocity);
	void free_voice();

	const Zynayumi& _zynayumi;

	// Map pitch (possibly several times the same) to a voice
	typedef std::multimap<unsigned char, Voice> Pitch2Voice;
	Pitch2Voice _voices;

	const unsigned char _max_voices;
};

} // ~namespace zynayumi

#endif
