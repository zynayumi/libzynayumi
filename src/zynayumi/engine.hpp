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
#include "../../ayumi/ayumi.h"
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

	/////////////////
   // Constants   //
   /////////////////

	static const int YM2149_CLOCK_RATE = 2000000;
	static const int AY8910_CLOCK_RATE = 1000000;

	///////////////////
	// Attributes    //
	///////////////////

	// Current ayumi state
	ayumi ay;

	// Current emulation mode (YM2149 or AY8910)
	EmulMode emulmode;

	// Current play mode
	PlayMode playmode;

	// Current buzzer shape
	Buzzer::Shape buzzershape;

	// Current ringmod loop
	RingMod::Loop ringmodloop;

	// Current ayumi envelope shape
	int ayenvshape;

	// Current pitches. Useful for handling chord based arp.
	std::multiset<unsigned char> pitches;

	// Stack of pitches, for mono mode
	std::vector<unsigned char> pitch_stack;

	// Pitches hold by the sustain pedal
	std::multiset<unsigned char> sustain_pitches;

	// Keep track of the previous pitch for portamento. Negative means
	// none.
	double previous_pitch;
	double last_pitch;

	const double lower_note_freq;
	int clock_rate;
	int sample_rate;             // Host sample rate
	double bpm;                  // Host beats per minute

	// Pitch wheel pitch
	double pw_pitch;

	// Modulation wheel CC depth (in semitone)
	double mw_depth;

	// Portamento time CC
	double portamento_time;

	// Volume CC gain
	float volume_gain;

	// Pan CC
	float pan;

	// Expression CC gain
	float expression_gain;

	// True iff the sustain pedal is on
	bool sustain_pedal;

	// Oversampling
	int oversampling;

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

	// Process audio.
	//
	// Assumptions:
	//
	// 1. The parameters do not change during audio processing
	//
	// 2. All processing is added to the buffers
	void audio_process(float* left_out, float* right_out,
	                   unsigned long sample_count);

	// Process MIDI events
	void note_on_process(unsigned char channel,
	                     unsigned char pitch,
	                     unsigned char velocity);
	void note_off_process(unsigned char channel, unsigned char pitch);
	void all_notes_off_process();
	void pitch_wheel_process(unsigned char channel, short value);
	void modulation_process(unsigned char channel, unsigned char value);
	void portamento_process(unsigned char channel, unsigned char value);
	void volume_process(unsigned char channel, unsigned char value);
	void pan_process(unsigned char channel, unsigned char value);
	void expression_process(unsigned char channel, unsigned char value);
	void sustain_pedal_process(unsigned char channel, unsigned char value);
	void enable_ym_channel(unsigned char ym_channel);
	void disable_ym_channel(unsigned char ym_channel);

	// Render to string the state of the engine. Convenient for
	// debugging.
	std::string to_string(const std::string& indent=std::string("  ")) const;

	double pitch2toneperiod(double pitch) const;
	int pitch2envperiod(double pitch) const;
	double freq2pitch(double freq) const;
	double smp2sec(unsigned long long smp_count) const;

	static float vol2gain(short value);

private:
	int select_ym_channel(bool poly) const;
	std::set<unsigned char> get_enabled_ym_channels() const;
	void set_last_pitch(unsigned char pitch);
	void add_voice(unsigned char pitch, unsigned char velocity);
	void add_all_voices(unsigned char pitch, unsigned char velocity);
	void set_all_voices_with_pitch(unsigned char pitch);
	void set_note_off_with_pitch(unsigned char pitch);
	void set_note_off_all_voices();
	void insert_pitch(unsigned char pitch);
	void erase_pitch(unsigned char pitch);
	void insert_sustain_pitch(unsigned char pitch);
	std::multiset<unsigned char>::iterator erase_sustain_pitch(unsigned char pitch);

	const Zynayumi& _zynayumi;

	// Vector of voices, one per ym channel
	typedef std::vector<Voice> Voices;
	Voices _voices;
};

} // ~namespace zynayumi

#endif
