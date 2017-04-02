/****************************************************************************
    
    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    zynayumi.cpp

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

#include <stdio.h>
#include "zynayumi.hpp"

using namespace zynayumi;

Zynayumi::Zynayumi() : engine(*this) {
	// Tweak patch for testing
	// PlayMode
	patch.playmode = PlayMode::RndArp;
	// Tone
	patch.tone.time = -1;
	// Noise
	patch.noise.time = 0.1;
	patch.noise.period = 16;
	// Env
	patch.ampenv.attack_level = 1;
	patch.ampenv.time1 = 10;
	patch.ampenv.level1 = 0;
	// patch.ampenv.time2 = 1;
	// patch.ampenv.level2 = 0.2;
	// patch.ampenv.time3 = 1;
	patch.ampenv.sustain_level = 0;
	patch.ampenv.release = 5;
	// Arp
	patch.arp.pitch1 = 0;
	patch.arp.pitch2 = 7;
	patch.arp.pitch3 = 12;
	patch.arp.freq = 30;
	patch.arp.repeat = 2;
	// LFO
	patch.lfo.freq = 10;
	patch.lfo.delay = 1;
	patch.lfo.depth = 0.1;
	// Portamento
	patch.port = 0.01;
	// Ring modulation
	patch.ringmod.waveform[0] = 1.0;
	patch.ringmod.waveform[1] = 0.9;
	patch.ringmod.waveform[2] = 0.8;
	patch.ringmod.waveform[3] = 0.7;
	patch.ringmod.waveform[4] = 0.6;
	patch.ringmod.waveform[5] = 0.5;
	patch.ringmod.waveform[6] = 0.4;
	patch.ringmod.waveform[7] = 0.3;
	patch.ringmod.detune = 0.01;
	patch.ringmod.minor = true;
	// Pitch envelope
	patch.pitchenv.attack_pitch = 0;
	patch.pitchenv.time = 2;
}

Zynayumi::~Zynayumi() {}

void Zynayumi::audio_process(float* left_out, float* right_out,
                             unsigned long sample_count) {
	engine.audio_process(left_out, right_out, sample_count);
}

void Zynayumi::noteOn_process(unsigned char channel,
                              unsigned char pitch,
                              unsigned char velocity) {
	dbg_printf("NOTE_ON: channel = %d, pitch = %d, velocity = %d\n",
	           channel, pitch, velocity);

	engine.noteOn_process(channel, pitch, velocity);
}

void Zynayumi::noteOff_process(unsigned char channel, unsigned char pitch) {
	dbg_printf("NOTE OFF: channel = %d, pitch = %d\n",
	           channel, pitch);

	engine.noteOff_process(channel, pitch);
}

void Zynayumi::sysex_process(unsigned length, unsigned char* data) {
	unsigned char command_ID;
	dbg_printf("SYSEX\n");
	for(unsigned i = 0; i < length; i++) dbg_printf("%X ", data[i]);
	dbg_printf("\n");
}

//print method
void Zynayumi::print() {
	printf("State of the engine :\n");
	engine.print(2);
}
