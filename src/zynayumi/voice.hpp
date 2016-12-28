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
	float env_level;            // Current envelope level

private:
	Engine& _engine;
	const Patch& _patch;

	unsigned char _pitch;              // Pitch possibly modulated by arp
	float _fine_pitch;                 // Like _pitch but continuous
	float _port_relative_pitch;        // Relative portamento relative pitch
	unsigned long long _env_smp_count; // Number of samples since note on or off
	unsigned long long _smp_count;     // Number of samples since note on
	float _actual_sustain_level;

	float linear_interpolate(float x1, float y1,
	                         float x2, float y2, float x) const;
	void update_env_level();
	void update_arp();
	void update_lfo();
	void update_port();
};

} // ~namespace zynayumi

#endif
