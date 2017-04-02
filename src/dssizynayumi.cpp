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
			// printf("events[e].type = %d\n", events[e].type);
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
	return NULL;
}

void DSSIZynayumi::select_program(unsigned long bank, unsigned long program) {
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
	zynayumi.patch.tone.detune = *m_ports[TONE_DETUNE];

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
	zynayumi.patch.ringmod.detune = *m_ports[RING_MOD_DETUNE];
	zynayumi.patch.ringmod.minor = (bool)std::round(*m_ports[RING_MOD_MINOR]);

	// Pitch LFO
	zynayumi.patch.lfo.freq = *m_ports[LFO_FREQ];
	zynayumi.patch.lfo.delay = *m_ports[LFO_DELAY];
	zynayumi.patch.lfo.depth = *m_ports[LFO_DEPTH];

	// Portamento
	zynayumi.patch.port = *m_ports[PORTAMENTO];
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
	ports.add_port(c_desc, "Play mode", i_desc | d_0, -0.1, 4.1);

	// Tone
	ports.add_port(c_desc, "Tone time", r_desc | d_min, -1.0, 10.0);
	ports.add_port(c_desc, "Tone detune", r_desc | d_0, -24.0, 24.0);

	// Noise
	ports.add_port(c_desc, "Noise time", r_desc | d_min, -1.0, 10.0);
	ports.add_port(c_desc, "Noise period", r_desc | d_middle, -0.1, 31.1);

	// Amplitude envelope
	ports.add_port(c_desc, "AmpEnv attack_level", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "AmpEnv time1", r_desc | d_0, 0.0, 10.0);
	ports.add_port(c_desc, "AmpEnv level1", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "AmpEnv time2", r_desc | d_0, 0.0, 10.0);
	ports.add_port(c_desc, "AmpEnv level2", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "AmpEnv time3", r_desc | d_0, 0.0, 10.0);
	ports.add_port(c_desc, "AmpEnv sustain_level", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "AmpEnv release", r_desc | d_0, 0.0, 10.0);

	// Pitch envelope
	ports.add_port(c_desc, "PitchEnv attack_pitch", r_desc | d_0, -96.0, 96.0);
	ports.add_port(c_desc, "PitchEnv time", r_desc | d_0, 0.0, 10.0);

	// Arpegio
	ports.add_port(c_desc, "Arp pitch1", r_desc | d_0, -48.0, 48.0);
	ports.add_port(c_desc, "Arp pitch2", r_desc | d_0, -48.0, 48.0);
	ports.add_port(c_desc, "Arp pitch3", r_desc | d_0, -48.0, 48.0);
	ports.add_port(c_desc, "Arp freq", r_desc | d_1, 0.0, 1000.0);
	ports.add_port(c_desc, "Arp repeat", i_desc | d_0, -0.1, 2.1);

	// Ring modulation
	ports.add_port(c_desc, "RingMod waveform_level1", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level2", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level3", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level4", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level5", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level6", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level7", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod waveform_level8", r_desc | d_1, 0.0, 1.0);
	ports.add_port(c_desc, "RingMod detune", r_desc | d_0, -24.0, 24);
	ports.add_port(c_desc, "RingMod minor", t_desc | d_1, -0.1, 1.1);

	// Pitch LFO
	ports.add_port(c_desc, "LFO freq", r_desc | d_1, 0.0, 1000.0);
	ports.add_port(c_desc, "LFO delay", r_desc | d_0, 0.0, 10.0);
	ports.add_port(c_desc, "LFO depth", r_desc | d_0, 0.0, 12.0);

	// Portamento
	ports.add_port(c_desc, "Portamento", r_desc | d_0, 0.0, 10.0);

	//////////////////
	// Audio output //
	//////////////////
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Left output");
	ports.add_port(LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT, "Right output");
  
	register_dssi<DSSIZynayumi>(1259, "Zynayumi-DSSI", 0, "Zynayumi DSSI plugin",
	                            "Nil Geisweiller", "GPL", ports);
}

