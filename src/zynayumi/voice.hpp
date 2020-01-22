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
	      unsigned char ym_channel, unsigned char pitch, unsigned char velocity);

	////////////////
	// Methods    //
	////////////////

	// Modifiers
	void set_note_off();
	void update();              // Update the voice state
	void set_note_pitch(unsigned char pitch);

	static double linear_interpolate(double x1, double y1,
	                                 double x2, double y2, double x);

	///////////////////
	// Attributes    //
	///////////////////

	int ym_channel;             // YM2149 channel
	unsigned char velocity;     // Note velocity
	unsigned char pitch;        // Note pitch
	bool note_on;
	double env_level;           // Current level, taking into account
	                            // amplitude envelope and velocity

private:
	// References are passed by pointer to please move assign operator
	Engine* _engine;
	const Patch* _patch;

	double _initial_pitch;             // Initial note pitch
	double _final_pitch;               // Final pitch after all pitch updates

	bool _tone_off;                    // True iff the tone is off
	bool _noise_off;                   // True iff the noise is off

	int _noise_period;                 // Final noise period, after
	                                   // account for noise period
	                                   // envelope

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
	unsigned _arp_rnd_offset_step;     // Random offset of the
	                                   // arp_step to allow different
	                                   // randomness cross voices
	unsigned _index;                   // Store the current random
	                                   // index to avoid repetition of
	                                   // 2 consecutive notes

	unsigned _env_smp_count;           // Number of samples since note on or off
	unsigned _smp_count;               // Number of samples since voice on
	double _time;                      // Time in second since voice on

	// Final level
	double _final_level;               // Current level, taking into account
	                                   // amplitude envelope, velocity and
	                                   // ring modulation

	// Ring Mod
	double _ringmod_smp_count;         // Number of samples since last
	                                   // ring modulation waveform
	                                   // cycle reset
	unsigned _ringmod_waveform_index;  // Current waveform index
	double _ringmod_waveform_level;    // Current waveform level
	double _ringmod_pitch;             // Pitch of ringmod
	double _ringmod_smp_period;        // Number of samples to make an
	                                   // entire cycle

	double _actual_sustain_level;

	bool _first_update;

	void update_pan();
	void update_tone_off();
	void update_noise_off();
	void update_noise_period();
	void update_pitchenv();
	void update_lfo();
	void update_arp();
	void update_port();
	void update_final_pitch();
	void update_ampenv();
	void update_ringmod();
	void update_ringmod_pitch();
	void update_ringmod_smp_period();
	void update_ringmod_smp_count();
	void update_ringmod_waveform_index();
	void update_ringmod_waveform_level();
	void sync_ringmod();
	void update_final_level();
};

} // ~namespace zynayumi

#endif
