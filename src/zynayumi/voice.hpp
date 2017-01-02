/****************************************************************************

    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    voice.hpp

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

#ifndef __ZYNAYUMI_VOICE_HPP
#define __ZYNAYUMI_VOICE_HPP

#include "patch.hpp"

namespace zynayumi {

class Engine;

/**
 * Holds information about the state of a voice (note, sample freq,
 * volume, etc)
 */

class Voice {
public:

	/////////////////////////////////
	// Constructors/descructors    //
	/////////////////////////////////

	Voice(Engine& engine, const Patch& patch,
	      unsigned char channel, unsigned char pitch, unsigned char velocity);

	////////////////
	// Methods    //
	////////////////

	// Modifiers
	void set_note_off();
	void update();              // Update the voice state
	void set_note_pitch(unsigned char pitch);

	///////////////////
	// Attributes    //
	///////////////////

	int channel;                // YM2149 channel
	// unsigned char pitch;        // Note pitch
	unsigned char velocity;     // Note velocity
	bool note_on;
	double env_level;           // Current level, taking into account
	                            // amplitude envelope and velocity

private:
	Engine& _engine;
	const Patch& _patch;

	double _note_pitch;                // Initial note pitch
	double _final_pitch;               // Final pitch after all pitch updates

	bool _t_off;                       // True iff the tone is off
	bool _n_off;                       // True iff the noise is off

	// Pitch envelope
	double _relative_pitchenv_pitch;   // Relative pitch envelope pitch

	// Portamento
	double _relative_port_pitch;       // Relative portamento pitch

	// LFO
	double _relative_lfo_pitch;        // Relative LFO pitch

	// Arpeggio
	unsigned _arp_step;                // Every time it increases the
	                                   // arp pitch changes
	double _relative_arp_pitch;        // Relative Arpeggio pitch

	unsigned _env_smp_count;           // Number of samples since note on or off
	unsigned _smp_count;               // Number of samples since voice on
	double _time;                      // Time in second since voice on

	// Final level
	double _final_level;               // Current level, taking into account
                                       // amplitude envelope, velocity and
                                       // ring modulation

	// Ring Mod
	double _ringmod_smp_count;         // Number of samples since last
                                       // ring modulation sample
                                       // change
	unsigned _ringmod_waveform_index;  // Current waveform index

	double _actual_sustain_level;

	double linear_interpolate(double x1, double y1,
	                          double x2, double y2, double x) const;
	void update_tone();
	void update_noise();
	void update_ampenv();
	void update_pitchenv();
	void update_arp();
	void update_lfo();
	void update_port();
	void calculate_final_pitch();
	void update_ring();
};

} // ~namespace zynayumi

#endif
