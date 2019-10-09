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

DSSIZynayumi::DSSIZynayumi(unsigned long frame_rate) {
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
			// printf("events[e].type = %d\nAMP_ENV_ATTACK_LEVEL_STR", events[e].type);
			switch(events[e].type) {
				//Note On, Off
			case SND_SEQ_EVENT_NOTEON :
				zynayumi.noteOn_process(0, events[e].data.note.note,
				                        events[e].data.note.velocity);
				break;
			case SND_SEQ_EVENT_NOTEOFF :
				zynayumi.noteOff_process(0, events[e].data.note.note);
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
	// Play mode
	zynayumi.patch.playmode = (PlayMode)std::round(*m_ports[PLAY_MODE]);

	// Tone
	zynayumi.patch.tone.time = *m_ports[TONE_TIME];
	zynayumi.patch.tone.detune = *m_ports[TONE_DETUNE] + *m_ports[TONE_TRANSPOSE];

	// Noise
	zynayumi.patch.noise.time = *m_ports[NOISE_TIME];
	zynayumi.patch.noise.period = (int)std::round(*m_ports[NOISE_PERIOD]);

	// Amplitude envelope
	zynayumi.patch.ampenv.attack_level = *m_ports[AMP_ENV_ATTACK_LEVEL];
	zynayumi.patch.ampenv.time1 = *m_ports[AMP_ENV_TIME1];
	zynayumi.patch.ampenv.level1 = *m_ports[AMP_ENV_LEVEL1];
	zynayumi.patch.ampenv.time2 = *m_ports[AMP_ENV_TIME2];
	zynayumi.patch.ampenv.level2 = *m_ports[AMP_ENV_LEVEL2];
	zynayumi.patch.ampenv.time3 = *m_ports[AMP_ENV_TIME3];
	zynayumi.patch.ampenv.sustain_level = *m_ports[AMP_ENV_SUSTAIN_LEVEL];
	zynayumi.patch.ampenv.release = *m_ports[AMP_ENV_RELEASE];

	// Pitch envelope
	zynayumi.patch.pitchenv.attack_pitch = *m_ports[PITCH_ENV_ATTACK_PITCH];
	zynayumi.patch.pitchenv.time = *m_ports[PITCH_ENV_TIME];

	// Arpegio
	zynayumi.patch.arp.pitch1 = *m_ports[ARP_PITCH1];
	zynayumi.patch.arp.pitch2 = *m_ports[ARP_PITCH2];
	zynayumi.patch.arp.pitch3 = *m_ports[ARP_PITCH3];
	zynayumi.patch.arp.freq = *m_ports[ARP_FREQ];
	zynayumi.patch.arp.repeat = (int)std::round(*m_ports[ARP_REPEAT]);

	// Ring modulation
	zynayumi.patch.ringmod.waveform[0] = *m_ports[RING_MOD_WAVEFORM_LEVEL1];
	zynayumi.patch.ringmod.waveform[1] = *m_ports[RING_MOD_WAVEFORM_LEVEL2];
	zynayumi.patch.ringmod.waveform[2] = *m_ports[RING_MOD_WAVEFORM_LEVEL3];
	zynayumi.patch.ringmod.waveform[3] = *m_ports[RING_MOD_WAVEFORM_LEVEL4];
	zynayumi.patch.ringmod.waveform[4] = *m_ports[RING_MOD_WAVEFORM_LEVEL5];
	zynayumi.patch.ringmod.waveform[5] = *m_ports[RING_MOD_WAVEFORM_LEVEL6];
	zynayumi.patch.ringmod.waveform[6] = *m_ports[RING_MOD_WAVEFORM_LEVEL7];
	zynayumi.patch.ringmod.waveform[7] = *m_ports[RING_MOD_WAVEFORM_LEVEL8];
	zynayumi.patch.ringmod.mirror = (bool)std::round(*m_ports[RING_MOD_MIRROR]);
	zynayumi.patch.ringmod.sync = (bool)std::round(*m_ports[RING_MOD_SYNC]);
	zynayumi.patch.ringmod.detune =
		*m_ports[RING_MOD_DETUNE] + *m_ports[RING_MOD_TRANSPOSE];

	// Pitch LFO
	zynayumi.patch.lfo.freq = *m_ports[LFO_FREQ];
	zynayumi.patch.lfo.delay = *m_ports[LFO_DELAY];
	zynayumi.patch.lfo.depth = *m_ports[LFO_DEPTH];

	// Portamento
	zynayumi.patch.port = *m_ports[PORTAMENTO];

	// Pan
	zynayumi.patch.pan.channel[0] = *m_ports[PAN_CHANNEL0];
	zynayumi.patch.pan.channel[1] = *m_ports[PAN_CHANNEL1];
	zynayumi.patch.pan.channel[2] = *m_ports[PAN_CHANNEL2];
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

	// Play mode
	ports.add_port(c_desc, PLAY_MODE_STR, i_desc | d_0, -0.1, 4.1);

	// Tone
	ports.add_port(c_desc, TONE_TIME_STR, r_desc | d_min, -1.0, 10.0);
	ports.add_port(c_desc, TONE_DETUNE_STR, r_desc | d_0,
	               TONE_DETUNE_MIN, TONE_DETUNE_MAX);
	ports.add_port(c_desc, TONE_TRANSPOSE_STR, i_desc | d_0,
	               TONE_TRANSPOSE_MIN - 0.1f, TONE_TRANSPOSE_MAX + 0.1);

	// Noise
	ports.add_port(c_desc, NOISE_TIME_STR, r_desc | d_0,
	               NOISE_TIME_MIN, NOISE_TIME_MAX);
	ports.add_port(c_desc, NOISE_PERIOD_STR, i_desc | d_middle,
	               NOISE_PERIOD_MIN - 0.1, NOISE_PERIOD_MAX + 0.1);

	// Amplitude envelope
	ports.add_port(c_desc, AMP_ENV_ATTACK_LEVEL_STR, r_desc | d_1,
	               AMP_ENV_ATTACK_LEVEL_MIN, AMP_ENV_ATTACK_LEVEL_MAX);
	ports.add_port(c_desc, AMP_ENV_TIME1_STR, r_desc | d_0,
	               AMP_ENV_TIME1_MIN, AMP_ENV_TIME1_MAX);
	ports.add_port(c_desc, AMP_ENV_LEVEL1_STR, r_desc | d_1,
	               AMP_ENV_LEVEL1_MIN, AMP_ENV_LEVEL1_MAX);
	ports.add_port(c_desc, AMP_ENV_TIME2_STR, r_desc | d_0,
	               AMP_ENV_TIME2_MIN, AMP_ENV_TIME2_MAX);
	ports.add_port(c_desc, AMP_ENV_LEVEL2_STR, r_desc | d_1,
	               AMP_ENV_LEVEL2_MIN, AMP_ENV_LEVEL2_MAX);
	ports.add_port(c_desc, AMP_ENV_TIME3_STR, r_desc | d_0,
	               AMP_ENV_TIME3_MIN, AMP_ENV_TIME3_MAX);
	ports.add_port(c_desc, AMP_ENV_SUSTAIN_LEVEL_STR, r_desc | d_1,
	               AMP_ENV_SUSTAIN_LEVEL_MIN, AMP_ENV_SUSTAIN_LEVEL_MAX);
	ports.add_port(c_desc, AMP_ENV_RELEASE_STR, r_desc | d_0,
	               AMP_ENV_RELEASE_MIN, AMP_ENV_RELEASE_MAX);

	// Pitch envelope
	ports.add_port(c_desc, PITCH_ENV_ATTACK_PITCH_STR, r_desc | d_0,
	               PITCH_ENV_ATTACK_PITCH_MIN, PITCH_ENV_ATTACK_PITCH_MAX);
	ports.add_port(c_desc, PITCH_ENV_TIME_STR, r_desc | d_0,
	               PITCH_ENV_TIME_MIN, PITCH_ENV_TIME_MAX);

	// Arpegio
	ports.add_port(c_desc, ARP_PITCH1_STR, r_desc | d_0,
	               ARP_PITCH1_MIN, ARP_PITCH1_MAX);
	ports.add_port(c_desc, ARP_PITCH2_STR, r_desc | d_0,
	               ARP_PITCH2_MIN, ARP_PITCH2_MAX);
	ports.add_port(c_desc, ARP_PITCH3_STR, r_desc | d_0,
	               ARP_PITCH3_MIN, ARP_PITCH3_MAX);
	ports.add_port(c_desc, ARP_FREQ_STR, r_desc | d_1,
	               ARP_FREQ_MIN, ARP_FREQ_MAX);
	ports.add_port(c_desc, ARP_REPEAT_STR, i_desc | d_0,
	               ARP_REPEAT_MIN - 0.1, ARP_REPEAT_MAX + 0.1);

	// Ring modulation
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL1_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL1_MIN, RING_MOD_WAVEFORM_LEVEL1_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL2_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL2_MIN, RING_MOD_WAVEFORM_LEVEL2_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL3_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL3_MIN, RING_MOD_WAVEFORM_LEVEL3_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL4_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL4_MIN, RING_MOD_WAVEFORM_LEVEL4_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL5_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL5_MIN, RING_MOD_WAVEFORM_LEVEL5_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL6_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL6_MIN, RING_MOD_WAVEFORM_LEVEL6_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL7_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL7_MIN, RING_MOD_WAVEFORM_LEVEL7_MAX);
	ports.add_port(c_desc, RING_MOD_WAVEFORM_LEVEL8_STR, r_desc | d_1,
	               RING_MOD_WAVEFORM_LEVEL8_MIN, RING_MOD_WAVEFORM_LEVEL8_MAX);
	ports.add_port(c_desc, RING_MOD_MIRROR_STR, t_desc | d_1,
	               RING_MOD_MIRROR_MIN - 0.1, RING_MOD_MIRROR_MAX + 0.1);
	ports.add_port(c_desc, RING_MOD_SYNC_STR, t_desc | d_1,
	               RING_MOD_SYNC_MIN - 0.1, RING_MOD_SYNC_MAX + 0.1);
	ports.add_port(c_desc, RING_MOD_DETUNE_STR, r_desc | d_0,
	               RING_MOD_DETUNE_MIN, RING_MOD_DETUNE_MAX);
	ports.add_port(c_desc, RING_MOD_TRANSPOSE_STR, i_desc | d_0,
	               RING_MOD_TRANSPOSE_MIN - 0.1, RING_MOD_TRANSPOSE_MAX + 0.1);

	// Pitch LFO
	ports.add_port(c_desc, LFO_FREQ_STR, r_desc | d_1,
	               LFO_FREQ_MIN, LFO_FREQ_MAX);
	ports.add_port(c_desc, LFO_DELAY_STR, r_desc | d_0,
	               LFO_DELAY_MIN, LFO_DELAY_MAX);
	ports.add_port(c_desc, LFO_DEPTH_STR, r_desc | d_0,
	               LFO_DEPTH_MIN, LFO_DEPTH_MAX);

	// Portamento
	ports.add_port(c_desc, PORTAMENTO_STR, r_desc | d_0, 0.0, 2.0);

	// Pan
	ports.add_port(c_desc, PAN_CHANNEL0_STR, r_desc | d_middle,
	               PAN_CHANNEL0_MIN, PAN_CHANNEL0_MAX);
	ports.add_port(c_desc, PAN_CHANNEL1_STR, r_desc | d_low,
	               PAN_CHANNEL1_MIN, PAN_CHANNEL1_MAX);
	ports.add_port(c_desc, PAN_CHANNEL2_STR, r_desc | d_high,
	               PAN_CHANNEL2_MIN, PAN_CHANNEL2_MAX);

	//////////////////
	// Audio output //
	//////////////////
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Left output");
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Right output");
  
	register_dssi<DSSIZynayumi>(1259, "Zynayumi-DSSI", 0, "Zynayumi (DSSI)",
	                            "Nil Geisweiller", "GPL", ports);
}
