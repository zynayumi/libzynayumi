/****************************************************************************
    
    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    zynayumi.hpp

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

#ifndef __ZYNAYUMI_ZYNAYUMI_HPP
#define __ZYNAYUMI_ZYNAYUMI_HPP

#include "patch.hpp"
#include "engine.hpp"
#include "presets.hpp"

// Set 1 if you want to print debug messages, 0 otherwise
#define ENABLE_PRINT_DEBUG 0

#if ENABLE_PRINT_DEBUG
#define dbg_printf(f, varargs...) printf(f, ##varargs)
#else
#define dbg_printf(f, varargs...)
#endif

namespace zynayumi {

class Zynayumi {

	///////////////////
	// Attributes    //
	///////////////////

public:

	// Presets
	Presets presets;

	// Current patch
	Patch patch;

	// Engine of the Zynayumi
	Engine engine;

	/////////////////////////////////
	// Constructors/descructors    //
	/////////////////////////////////

	Zynayumi();
	~Zynayumi();

	////////////////
	// Methods    //
	////////////////

	// Set/get host sample rate
	void set_sample_rate(int sr);
	unsigned get_sample_rate() const;

	// Get/set host BPM
	void set_bpm(double bpm);
	double get_bpm() const;

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

	// to_string method for debugging
	std::string to_string(const std::string& indent) const;
};

} // ~namespace zynayumi

#endif
