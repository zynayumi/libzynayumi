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

	Voice(Engine& engine, const Patch& patch, unsigned char ym_channel);
	~Voice();

	////////////////
	// Methods    //
	////////////////

	// Modifiers
	void set_note_on(unsigned char pitch, unsigned char velocity);
	void set_note_pitch(unsigned char pitch);
	void set_velocity(unsigned char velocity);
	void set_note_off();
	void retrig();
	void enable();
	void disable();
	void silence();
	bool is_silent() const;
	void update();              // Update the voice state

	static double linear_interpolate(double x1, double y1,
	                                 double x2, double y2,
	                                 double x);

	static double logistic_interpolate(double x1, double y1,
	                                   double x2, double y2,
	                                   double x, double scale);

	static double exponential_decay_interpolate(double x1, double y1,
	                                            double x2, double y2,
	                                            double x, double exponent);

	///////////////////
	// Attributes    //
	///////////////////

	bool enabled;               // Whether the voice is enabled
	int ym_channel;             // YM2149 channel
	unsigned char velocity;     // Note velocity
	double velocity_level;      // Corresponding velocity level
	unsigned char pitch;        // Note pitch
	bool note_on;               // True iff key is pressed
	double env_level;           // Current level, taking into account
	                            // amplitude envelope and velocity
	double on_time;             // Time in second since voice on
	double pitch_time;          // Time in second since new pitch change

private:
	// References are passed by pointer to please move assign operator
	Engine* _engine;
	const Patch* _patch;

	double _initial_pitch;             // Initial note pitch

	double _final_pitch;               // Final pitch after all pitch
	                                   // updates

	bool _tone_off;                    // True iff the tone is off

	bool _noise_off;                   // True iff the noise is off

	bool _buzzer_off;                  // True iff the buzzer is off

	int _noise_period;                 // Final noise period, after
	                                   // account for noise period
	                                   // envelope

	// Pitch envelope
	double _relative_pitchenv_pitch;   // Relative pitch envelope pitch

	// Portamento
	double _relative_port_pitch;       // Relative portamento pitch

	// LFO
	double _relative_lfo_pitch;        // Relative LFO pitch

	// Sequencer
	int _seq_step;                     // Total number of steps since
	                                   // the note is on. Negative
	                                   // means undefined.

	bool _seq_change;                  // Whether step has incremented

	int _seq_index;                    // Step index, range from -1 to
	                                   // 15. Negative means undefined.

	double _seq_level;                 // Volume level determined by
	                                   // the sequencer.

	double _relative_seq_pitch;        // Relative Arpeggio pitch

	unsigned _seq_rnd_offset_step;     // Random offset of the
	                                   // seq_step to allow different
	                                   // randomness cross voices

	unsigned _rnd_index;               // Store the current random
	                                   // index to avoid repetition of
	                                   // 2 consecutive notes

	unsigned _env_smp_count;           // Number of samples since note
	                                   // on or off

	unsigned _on_smp_count;            // Number of samples since voice
	                                   // on

	unsigned _pitch_smp_count;         // Number of samples since pitch
	                                   // change

	// Final level
	double _final_level;               // Current level, taking into account
	                                   // amplitude envelope, velocity and
	                                   // ring modulation

	// Ring Mod
	double _ringmod_smp_count;         // Number of samples since last
	                                   // ring modulation waveform
	                                   // cycle reset
	bool _ringmod_back;                // Whether ringmod waveform is
                                      // going backward
	unsigned _ringmod_waveform_index;  // Current waveform index
	double _ringmod_waveform_level;    // Current waveform level
	double _ringmod_pitch;             // Pitch of ringmod
	double _ringmod_smp_period;        // Number of samples to make a
	                                   // ringmod waveform segment
	double _ringmod_whole_smp_period;  // Number of samples to make the
	                                   // whole ringmod waveform

	double _buzzer_pitch;              // Pitch of the buzzer
	int _buzzer_period;                // Period of the buzzer

	double _actual_sustain_level;

	bool _first_update;

	bool _tone_trigger;                // True iff tone goes from 0 to 1
	int _last_tone;

	/**
	 * Return the spread, null for the first channel, negative for the
	 * second, positive for the third.
	 */
	double ym_channel_to_spread() const;

	void update_pan();
	void update_seq();
	void update_tone();
	void update_tone_off();
	void update_noise_off();
	void update_noise_period();
	void update_pitchenv();
	void update_lfo();
	void update_arp();
	void update_portamento();
	void update_final_pitch();
	void update_env();
	void update_ringmod();
	void update_ringmod_pitch();
	void update_ringmod_smp_period();
	void update_ringmod_smp_count();
	void update_ringmod_waveform_index();
	void update_ringmod_waveform_level();
	void update_buzzer();
	void update_buzzer_off();
	void update_buzzer_pitch();
	void update_buzzer_period();
	void update_buzzer_shape();
	void update_seq_level();
	void update_final_level();
	void reset_tone();
	void reset_ringmod();
	void reset_buzzer();
	void sync_ringmod();
	void sync_buzzer();

	static double velocity_to_level(double velocity_sensitivity,
	                                unsigned char velocity);
	static double velocity_to_depth(double velocity_sensitivity,
	                                unsigned char velocity);

	static double lfo_cycle_remainder(double freq, double time);
	static double lfo_sine_pitch(double freq, double time);
	static double lfo_triangle_pitch(double freq, double time);
	static double lfo_downsaw_pitch(double freq, double time);
	static double lfo_upsaw_pitch(double freq, double time);
	static double lfo_square_pitch(double freq, double time);
	static double lfo_rand_pitch(double freq, double time);

	static uint32_t hash(uint32_t a);
	static double normalize_level(int level);
};

} // ~namespace zynayumi

#endif
