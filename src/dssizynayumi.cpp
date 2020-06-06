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
	ports.add_port(c_desc, TONE_TIME_NAME, r_desc | d_min,
	               TONE_TIME_L_ALT, TONE_TIME_U_ALT);
	ports.add_port(c_desc, TONE_DETUNE_NAME, r_desc | d_0,
	               TONE_DETUNE_L, TONE_DETUNE_U);
	ports.add_port(c_desc, TONE_TRANSPOSE_NAME, i_desc | d_0,
	               TONE_TRANSPOSE_L - delta, TONE_TRANSPOSE_U + delta);
	ports.add_port(c_desc, TONE_SPREAD_NAME, r_desc | d_0,
	               TONE_SPREAD_L, TONE_SPREAD_U);

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

	// Amplitude envelope
	ports.add_port(c_desc, AMP_ENV_ATTACK_TIME_NAME, r_desc | d_0,
	               AMP_ENV_ATTACK_TIME_L, AMP_ENV_ATTACK_TIME_U);
	ports.add_port(c_desc, AMP_ENV_HOLD1_LEVEL_NAME, r_desc | d_1,
	               AMP_ENV_HOLD1_LEVEL_L, AMP_ENV_HOLD1_LEVEL_U);
	ports.add_port(c_desc, AMP_ENV_INTER1_TIME_NAME, r_desc | d_0,
	               AMP_ENV_INTER1_TIME_L, AMP_ENV_INTER1_TIME_U);
	ports.add_port(c_desc, AMP_ENV_HOLD2_LEVEL_NAME, r_desc | d_1,
	               AMP_ENV_HOLD2_LEVEL_L, AMP_ENV_HOLD2_LEVEL_U);
	ports.add_port(c_desc, AMP_ENV_INTER2_TIME_NAME, r_desc | d_0,
	               AMP_ENV_INTER2_TIME_L, AMP_ENV_INTER2_TIME_U);
	ports.add_port(c_desc, AMP_ENV_HOLD3_LEVEL_NAME, r_desc | d_1,
	               AMP_ENV_HOLD3_LEVEL_L, AMP_ENV_HOLD3_LEVEL_U);
	ports.add_port(c_desc, AMP_ENV_DECAY_TIME_NAME, r_desc | d_0,
	               AMP_ENV_DECAY_TIME_L, AMP_ENV_DECAY_TIME_U);
	ports.add_port(c_desc, AMP_ENV_SUSTAIN_LEVEL_NAME, r_desc | d_1,
	               AMP_ENV_SUSTAIN_LEVEL_L, AMP_ENV_SUSTAIN_LEVEL_U);
	ports.add_port(c_desc, AMP_ENV_RELEASE_NAME, r_desc | d_0,
	               AMP_ENV_RELEASE_L, AMP_ENV_RELEASE_U);

	// Pitch envelope
	ports.add_port(c_desc, PITCH_ENV_ATTACK_PITCH_NAME, r_desc | d_0,
	               PITCH_ENV_ATTACK_PITCH_L, PITCH_ENV_ATTACK_PITCH_U);
	ports.add_port(c_desc, PITCH_ENV_TIME_NAME, r_desc | d_0,
	               PITCH_ENV_TIME_L, PITCH_ENV_TIME_U);

	// Arpegio
	ports.add_port(c_desc, ARP_PITCH1_NAME, i_desc | d_0,
	               ARP_PITCH1_L - delta, ARP_PITCH1_U + delta);
	ports.add_port(c_desc, ARP_PITCH2_NAME, i_desc | d_0,
	               ARP_PITCH2_L - delta, ARP_PITCH2_U + delta);
	ports.add_port(c_desc, ARP_PITCH3_NAME, i_desc | d_0,
	               ARP_PITCH3_L - delta, ARP_PITCH3_U + delta);
	ports.add_port(c_desc, ARP_BEAT_DIVISOR_NAME, i_desc | d_low,
	               ARP_BEAT_DIVISOR_L - delta, ARP_BEAT_DIVISOR_U + delta);
	ports.add_port(c_desc, ARP_BEAT_MULTIPLIER_NAME, i_desc | d_1,
	               ARP_BEAT_MULTIPLIER_L - delta, ARP_BEAT_MULTIPLIER_U + delta);
	ports.add_port(c_desc, ARP_REPEAT_NAME, i_desc | d_0,
	               ARP_REPEAT_L - delta, ARP_REPEAT_U + delta);

	// Ring modulation
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL1_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL1_L, RING_MOD_WAVEFORM_LEVEL1_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL2_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL2_L, RING_MOD_WAVEFORM_LEVEL2_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL3_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL3_L, RING_MOD_WAVEFORM_LEVEL3_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL4_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL4_L, RING_MOD_WAVEFORM_LEVEL4_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL5_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL5_L, RING_MOD_WAVEFORM_LEVEL5_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL6_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL6_L, RING_MOD_WAVEFORM_LEVEL6_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL7_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL7_L, RING_MOD_WAVEFORM_LEVEL7_U);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL8_NAME, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL8_L, RING_MOD_WAVEFORM_LEVEL8_U);
	ports.add_port(c_desc, RING_MOD_MIRROR_NAME, t_desc | d_1,
	               RING_MOD_MIRROR_L - delta, RING_MOD_MIRROR_U + delta);
	ports.add_port(c_desc, RING_MOD_SYNC_NAME, t_desc | d_1,
	               RING_MOD_SYNC_L - delta, RING_MOD_SYNC_U + delta);
	ports.add_port(c_desc, RING_MOD_DETUNE_NAME, r_desc | d_0,
	               RING_MOD_DETUNE_L, RING_MOD_DETUNE_U);
	ports.add_port(c_desc, RING_MOD_TRANSPOSE_NAME, i_desc | d_0,
	               RING_MOD_TRANSPOSE_L - delta, RING_MOD_TRANSPOSE_U + delta);
	ports.add_port(c_desc, RING_MOD_FIXED_FREQUENCY_NAME, i_desc | d_1,
	               RING_MOD_FIXED_FREQUENCY_L - delta, RING_MOD_FIXED_FREQUENCY_U + delta);
	ports.add_port(c_desc, RING_MOD_FIXED_VS_RELATIVE_NAME, r_desc | d_1,
	               RING_MOD_FIXED_VS_RELATIVE_L, RING_MOD_FIXED_VS_RELATIVE_U);

	// Pitch LFO
	ports.add_port(c_desc, LFO_FREQ_NAME, r_desc | d_low,
	               LFO_FREQ_L, LFO_FREQ_U);
	ports.add_port(c_desc, LFO_DELAY_NAME, r_desc | d_0,
	               LFO_DELAY_L, LFO_DELAY_U);
	ports.add_port(c_desc, LFO_DEPTH_NAME, r_desc | d_0,
	               LFO_DEPTH_L, LFO_DEPTH_U);

	// Portamento
	ports.add_port(c_desc, PORTAMENTO_TIME_NAME, r_desc | d_0,
	               PORTAMENTO_TIME_L, PORTAMENTO_TIME_U);

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
