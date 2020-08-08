/****************************************************************************
    
    DSSI wrapper for Zynayumi

    dssizynayumi.cpp

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

#include <cmath>

#include "dssizynayumi.hpp"

using namespace zynayumi;
using namespace std;

DSSIZynayumi::DSSIZynayumi(unsigned long sample_rate)
	: parameters(zynayumi)
{
	zynayumi.set_sample_rate(sample_rate);
}

void DSSIZynayumi::run_synth(unsigned long sample_count,
                             snd_seq_event_t* events,
                             unsigned long event_count) {
	update_patch();

	LADSPA_Data* left_out = m_ports[LEFT_OUTPUT];
	LADSPA_Data* right_out = m_ports[RIGHT_OUTPUT];

	unsigned long e = 0; //current event in the loop
	unsigned long t = 0; //current tick
	unsigned long net = 0; //tick of the next event

	// handle any MIDI events that occur in this frame
	while(e < event_count) {
		if(events[e].time.tick == t) {
			switch(events[e].type) {
				//Note On, Off
			case SND_SEQ_EVENT_NOTEON :
				zynayumi.note_on_process(0, events[e].data.note.note,
				                         events[e].data.note.velocity);
				break;
			case SND_SEQ_EVENT_NOTEOFF :
				zynayumi.note_off_process(0, events[e].data.note.note);
				break;
				//Sysex
			case SND_SEQ_EVENT_SYSEX :
				zynayumi.sysex_process(events[e].data.ext.len,
				                       (unsigned char*) events[e].data.ext.ptr);
				break;
			default:
				break;
			}
			e++;
		}
		else {
			net = events[e].time.tick;
			zynayumi.audio_process(&left_out[t], &right_out[t], net - t);
			t = net;
		}
	}
	zynayumi.audio_process(&left_out[net], &right_out[net], sample_count - net);
}

int DSSIZynayumi::get_midi_controller_for_port(unsigned long port) {
	return DSSI_NONE;
}

const DSSI_Program_Descriptor* DSSIZynayumi::get_program(unsigned long index) {
	if (index < m_progs.size()) {
		return &m_progs[index];
	}
	return NULL;
}

void DSSIZynayumi::select_program(unsigned long bank, unsigned long program) {
	// For now bank is ignored and program corresponds to the index
}

char* DSSIZynayumi::configure(const char* key, const char* value) {
	return NULL;
}

void DSSIZynayumi::update_patch()
{
	for (std::size_t pi = 0; pi < (std::size_t)PARAMETERS_COUNT; pi++)
		parameters.set_value((ParameterIndex)pi, *m_ports[pi]);
}

void initialise_2() __attribute__((constructor));

void initialise_2() {
	DSSIPortList ports;
  
	//////////////////////
	// Control ports    //
	//////////////////////
	const LADSPA_PortDescriptor c_desc = LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT;
	const LADSPA_PortRangeHintDescriptor r_desc = LADSPA_HINT_BOUNDED_BELOW |
		LADSPA_HINT_BOUNDED_ABOVE;
	const LADSPA_PortRangeHintDescriptor l_desc = LADSPA_HINT_BOUNDED_BELOW |
		LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_LOGARITHMIC;
	const LADSPA_PortRangeHintDescriptor i_desc = LADSPA_HINT_BOUNDED_BELOW |
		LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_INTEGER;
	const LADSPA_PortRangeHintDescriptor t_desc = LADSPA_HINT_TOGGLED;
	const LADSPA_PortRangeHintDescriptor d_min = LADSPA_HINT_DEFAULT_MINIMUM;
	const LADSPA_PortRangeHintDescriptor d_max = LADSPA_HINT_DEFAULT_MAXIMUM;
	const LADSPA_PortRangeHintDescriptor d_middle = LADSPA_HINT_DEFAULT_MIDDLE;
	const LADSPA_PortRangeHintDescriptor d_low = LADSPA_HINT_DEFAULT_LOW;
	const LADSPA_PortRangeHintDescriptor d_high = LADSPA_HINT_DEFAULT_HIGH;
	const LADSPA_PortRangeHintDescriptor d_0 = LADSPA_HINT_DEFAULT_0;
	const LADSPA_PortRangeHintDescriptor d_1 = LADSPA_HINT_DEFAULT_1;
	const LADSPA_PortRangeHintDescriptor d_100 = LADSPA_HINT_DEFAULT_100;
	const LADSPA_PortRangeHintDescriptor d_440 = LADSPA_HINT_DEFAULT_440;

	const double delta = 0.001;

	// Emulation mode
	ports.add_port(c_desc, EMUL_MODE_NAME, i_desc | d_0,
	               -delta, (double)EmulMode::Count -1.0 + delta);

	// Play mode
	ports.add_port(c_desc, PLAY_MODE_NAME, i_desc | d_0,
	               -delta, (double)PlayMode::Count -1.0 + delta);

	// Tone
	ports.add_port(c_desc, TONE_SYNC_NAME, t_desc | d_1,
	               TONE_SYNC_L - delta, TONE_SYNC_U + delta);
	ports.add_port(c_desc, TONE_PHASE_NAME, r_desc | d_0,
	               TONE_PHASE_L - delta, TONE_PHASE_U + delta);
	ports.add_port(c_desc, TONE_TIME_NAME, r_desc | d_min,
	               TONE_TIME_L_ALT, TONE_TIME_U_ALT);
	ports.add_port(c_desc, TONE_DETUNE_NAME, r_desc | d_0,
	               TONE_DETUNE_L, TONE_DETUNE_U);
	ports.add_port(c_desc, TONE_TRANSPOSE_NAME, i_desc | d_0,
	               TONE_TRANSPOSE_L - delta, TONE_TRANSPOSE_U + delta);
	ports.add_port(c_desc, TONE_SPREAD_NAME, r_desc | d_0,
	               TONE_SPREAD_L, TONE_SPREAD_U);
	ports.add_port(c_desc, TONE_LEGACY_TUNING_NAME, i_desc | d_0,
	               TONE_LEGACY_TUNING_L - delta, TONE_LEGACY_TUNING_U + delta);

	// Noise
	ports.add_port(c_desc, NOISE_TIME_NAME, r_desc | d_0,
	               NOISE_TIME_L_ALT, NOISE_TIME_U_ALT);
	ports.add_port(c_desc, NOISE_PERIOD_NAME, i_desc | d_1,
	               NOISE_PERIOD_L - delta, NOISE_PERIOD_U + delta);

	// Noise Period Envelope
	ports.add_port(c_desc, NOISE_PERIOD_ENV_ATTACK_NAME, i_desc | d_1,
	               NOISE_PERIOD_ENV_ATTACK_L - delta, NOISE_PERIOD_ENV_ATTACK_U + delta);
	ports.add_port(c_desc, NOISE_PERIOD_ENV_TIME_NAME, r_desc | d_0,
	               NOISE_PERIOD_ENV_TIME_L, NOISE_PERIOD_ENV_TIME_U);

	// Envelope
	ports.add_port(c_desc, ENV_ATTACK_TIME_NAME, r_desc | d_0,
	               ENV_ATTACK_TIME_L, ENV_ATTACK_TIME_U);
	ports.add_port(c_desc, ENV_HOLD1_LEVEL_NAME, r_desc | d_1,
	               ENV_HOLD1_LEVEL_L, ENV_HOLD1_LEVEL_U);
	ports.add_port(c_desc, ENV_INTER1_TIME_NAME, r_desc | d_0,
	               ENV_INTER1_TIME_L, ENV_INTER1_TIME_U);
	ports.add_port(c_desc, ENV_HOLD2_LEVEL_NAME, r_desc | d_1,
	               ENV_HOLD2_LEVEL_L, ENV_HOLD2_LEVEL_U);
	ports.add_port(c_desc, ENV_INTER2_TIME_NAME, r_desc | d_0,
	               ENV_INTER2_TIME_L, ENV_INTER2_TIME_U);
	ports.add_port(c_desc, ENV_HOLD3_LEVEL_NAME, r_desc | d_1,
	               ENV_HOLD3_LEVEL_L, ENV_HOLD3_LEVEL_U);
	ports.add_port(c_desc, ENV_DECAY_TIME_NAME, r_desc | d_0,
	               ENV_DECAY_TIME_L, ENV_DECAY_TIME_U);
	ports.add_port(c_desc, ENV_SUSTAIN_LEVEL_NAME, r_desc | d_1,
	               ENV_SUSTAIN_LEVEL_L, ENV_SUSTAIN_LEVEL_U);
	ports.add_port(c_desc, ENV_RELEASE_NAME, r_desc | d_0,
	               ENV_RELEASE_L, ENV_RELEASE_U);

	// Pitch envelope
	ports.add_port(c_desc, PITCH_ENV_ATTACK_PITCH_NAME, r_desc | d_0,
	               PITCH_ENV_ATTACK_PITCH_L, PITCH_ENV_ATTACK_PITCH_U);
	ports.add_port(c_desc, PITCH_ENV_TIME_NAME, r_desc | d_0,
	               PITCH_ENV_TIME_L, PITCH_ENV_TIME_U);

	// Sequencer
	ports.add_port(c_desc, SEQ_TONE_PITCH_0_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_0_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_0_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_0_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_1_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_1_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_1_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_1_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_2_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_2_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_2_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_2_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_3_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_3_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_3_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_3_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_4_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_4_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_4_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_4_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_5_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_5_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_5_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_5_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_6_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_6_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_6_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_6_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_7_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_7_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_7_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_7_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_8_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_8_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_8_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_8_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_9_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_9_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_9_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_9_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_10_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_10_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_10_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_10_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_11_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_11_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_11_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_11_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_12_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_12_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_12_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_12_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_13_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_13_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_13_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_13_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_14_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_14_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_14_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_14_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TONE_PITCH_15_NAME, i_desc | d_0,
	               SEQ_TONE_PITCH_L - delta, SEQ_TONE_PITCH_U + delta);
	ports.add_port(c_desc, SEQ_NOISE_PERIOD_15_NAME, i_desc | d_0,
	               SEQ_NOISE_PERIOD_L - delta, SEQ_NOISE_PERIOD_U + delta);
	ports.add_port(c_desc, SEQ_RINGMOD_DEPTH_15_NAME, r_desc | d_max,
	               SEQ_RINGMOD_DEPTH_L - delta, SEQ_RINGMOD_DEPTH_U + delta);
	ports.add_port(c_desc, SEQ_LEVEL_15_NAME, i_desc | d_max,
	               SEQ_LEVEL_L - delta, SEQ_LEVEL_U + delta);
	ports.add_port(c_desc, SEQ_TEMPO_NAME, r_desc | d_low,
	               SEQ_TEMPO_L, SEQ_TEMPO_U);
	ports.add_port(c_desc, SEQ_HOST_SYNC_NAME, i_desc | d_1,
	               SEQ_HOST_SYNC_L - delta, SEQ_HOST_SYNC_U + delta);
	ports.add_port(c_desc, SEQ_BEAT_DIVISOR_NAME, i_desc | d_low,
	               SEQ_BEAT_DIVISOR_L - delta, SEQ_BEAT_DIVISOR_U + delta);
	ports.add_port(c_desc, SEQ_BEAT_MULTIPLIER_NAME, i_desc | d_1,
	               SEQ_BEAT_MULTIPLIER_L - delta, SEQ_BEAT_MULTIPLIER_U + delta);
	ports.add_port(c_desc, SEQ_LOOP_NAME, i_desc | d_0,
	               SEQ_LOOP_L - delta, SEQ_LOOP_U + delta);
	ports.add_port(c_desc, SEQ_END_NAME, i_desc | d_0,
	               SEQ_END_L - delta, SEQ_END_U + delta);

	// Ring modulation
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL1_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL1_L, RINGMOD_WAVEFORM_LEVEL1_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL2_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL2_L, RINGMOD_WAVEFORM_LEVEL2_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL3_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL3_L, RINGMOD_WAVEFORM_LEVEL3_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL4_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL4_L, RINGMOD_WAVEFORM_LEVEL4_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL5_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL5_L, RINGMOD_WAVEFORM_LEVEL5_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL6_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL6_L, RINGMOD_WAVEFORM_LEVEL6_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL7_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL7_L, RINGMOD_WAVEFORM_LEVEL7_U);
	ports.add_port(c_desc, RINGMOD_WAVEFORM_LEVEL8_NAME, r_desc | d_1,
	               RINGMOD_WAVEFORM_LEVEL8_L, RINGMOD_WAVEFORM_LEVEL8_U);
	ports.add_port(c_desc, RINGMOD_SYNC_NAME, t_desc | d_1,
	               RINGMOD_SYNC_L - delta, RINGMOD_SYNC_U + delta);
	ports.add_port(c_desc, RINGMOD_PHASE_NAME, r_desc | d_0,
	               RINGMOD_PHASE_L - delta, RINGMOD_PHASE_U + delta);
	ports.add_port(c_desc, RINGMOD_MIRROR_NAME, t_desc | d_1,
	               RINGMOD_MIRROR_L - delta, RINGMOD_MIRROR_U + delta);
	ports.add_port(c_desc, RINGMOD_DETUNE_NAME, r_desc | d_0,
	               RINGMOD_DETUNE_L, RINGMOD_DETUNE_U);
	ports.add_port(c_desc, RINGMOD_TRANSPOSE_NAME, i_desc | d_0,
	               RINGMOD_TRANSPOSE_L - delta, RINGMOD_TRANSPOSE_U + delta);
	ports.add_port(c_desc, RINGMOD_FIXED_FREQUENCY_NAME, i_desc | d_1,
	               RINGMOD_FIXED_FREQUENCY_L - delta, RINGMOD_FIXED_FREQUENCY_U + delta);
	ports.add_port(c_desc, RINGMOD_FIXED_VS_RELATIVE_NAME, r_desc | d_1,
	               RINGMOD_FIXED_VS_RELATIVE_L, RINGMOD_FIXED_VS_RELATIVE_U);
	ports.add_port(c_desc, RINGMOD_DEPTH_NAME, r_desc | d_1,
	               RINGMOD_DEPTH_L, RINGMOD_DEPTH_U);

	// Buzzer
	ports.add_port(c_desc, BUZZER_SHAPE_NAME, i_desc | d_0,
	               -delta, (double)Buzzer::Shape::Count -1.0 + delta);
	ports.add_port(c_desc, BUZZER_SYNC_NAME, t_desc | d_1,
	               BUZZER_SYNC_L - delta, BUZZER_SYNC_U + delta);
	ports.add_port(c_desc, BUZZER_PHASE_NAME, r_desc | d_0,
	               BUZZER_PHASE_L - delta, BUZZER_PHASE_U + delta);
	ports.add_port(c_desc, BUZZER_TIME_NAME, r_desc | d_0,
	               BUZZER_TIME_L_ALT, BUZZER_TIME_U_ALT);
	ports.add_port(c_desc, BUZZER_DETUNE_NAME, r_desc | d_0,
	               BUZZER_DETUNE_L, BUZZER_DETUNE_U);
	ports.add_port(c_desc, BUZZER_TRANSPOSE_NAME, i_desc | d_0,
	               BUZZER_TRANSPOSE_L - delta, BUZZER_TRANSPOSE_U + delta);
	
	// Pitch LFO
	ports.add_port(c_desc, LFO_SHAPE_NAME, i_desc | d_0,
	               -delta, (double)LFO::Shape::Count -1.0 + delta);
	ports.add_port(c_desc, LFO_FREQ_NAME, r_desc | d_low,
	               LFO_FREQ_L, LFO_FREQ_U);
	ports.add_port(c_desc, LFO_DELAY_NAME, r_desc | d_0,
	               LFO_DELAY_L, LFO_DELAY_U);
	ports.add_port(c_desc, LFO_DEPTH_NAME, r_desc | d_0,
	               LFO_DEPTH_L, LFO_DEPTH_U);

	// Portamento
	ports.add_port(c_desc, PORTAMENTO_TIME_NAME, r_desc | d_0,
	               PORTAMENTO_TIME_L, PORTAMENTO_TIME_U);
	ports.add_port(c_desc, PORTAMENTO_SMOOTHNESS_NAME, r_desc | d_1,
	               PORTAMENTO_SMOOTHNESS_L, PORTAMENTO_SMOOTHNESS_U);

	// Gain
	ports.add_port(c_desc, GAIN_NAME, r_desc | d_1, GAIN_L, GAIN_U);

	// Pan
	ports.add_port(c_desc, PAN0_NAME, r_desc | d_middle,
	               PAN0_L, PAN0_U);
	ports.add_port(c_desc, PAN1_NAME, r_desc | d_low,
	               PAN1_L, PAN1_U);
	ports.add_port(c_desc, PAN2_NAME, r_desc | d_high,
	               PAN2_L, PAN2_U);

	// Control
	ports.add_port(c_desc, PITCH_WHEEL_NAME, i_desc | d_low,
	               PITCH_WHEEL_L - delta, PITCH_WHEEL_U + delta);

	ports.add_port(c_desc, VELOCITY_SENSITIVITY_NAME, r_desc | d_middle,
	               VELOCITY_SENSITIVITY_L, VELOCITY_SENSITIVITY_U);

	ports.add_port(c_desc, MODULATION_SENSITIVITY_NAME, r_desc | d_middle,
	               MODULATION_SENSITIVITY_L, MODULATION_SENSITIVITY_U);	

	//////////////////
	// Audio output //
	//////////////////
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Left output");
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Right output");
  
	register_dssi<DSSIZynayumi>(1259, "Zynayumi-DSSI", 0, "Zynayumi",
	                            "Nil Geisweiller", "GPL", ports);
}
