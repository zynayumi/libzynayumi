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

	Voice(Engine& engine,
	      const Patch& patch, unsigned char pitch, unsigned char velocity);

	////////////////
	// Methods    //
	////////////////

	// Modifiers
	void set_note_off();
	void update();              // Update the voice state

	///////////////////
	// Attributes    //
	///////////////////

	unsigned char pitch;
	unsigned char velocity;
	bool note_on;
	double env_level;           // Current envelope level, taking into
	                            // account velocity
	double level;               // Current level, taking into account
                                // envelope, velocity and ring
                                // modulation

private:
	Engine& _engine;
	const Patch& _patch;

	unsigned char _pitch;              // Pitch possibly modulated by arp
	double _fine_pitch;                // Like _pitch but continuous

	// Portamento
	double _port_relative_pitch;       // Relative portamento relative pitch
	double _port_fine_pitch;           // Absolute portamento pitch

	unsigned _env_smp_count;           // Number of samples since note on or off
	unsigned _smp_count;               // Number of samples since note on

	// Ring Mod
	double _ringmod_smp_count;         // Number of samples since last
                                       // ring modulation sample
                                       // change
	unsigned _ringmod_waveform_index;  // Current waveform index

	double _actual_sustain_level;

	double linear_interpolate(double x1, double y1,
	                          double x2, double y2, double x) const;
	void update_env_level();
	void update_arp();
	void update_lfo();
	void update_port();
	void update_ring();
};

} // ~namespace zynayumi

#endif
