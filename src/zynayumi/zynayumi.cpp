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
	patch.playmode = PlayMode::DownArp;
	// Noise
	patch.noise.time = 0;
	// Env
	patch.env.attack_level = 1;
	// patch.env.time1 = 1;
	// patch.env.level1 = 0;
	// patch.env.time2 = 1;
	// patch.env.level2 = 0.5;
	// patch.env.time3 = 1;
	// patch.env.sustain_level = 0.25;
	patch.env.release = 5;
	// Arp
	patch.arp.freq = 20;
	// LFO
	patch.lfo.freq = 8;
	patch.lfo.delay = 4;
	// patch.lfo.depth = 1;
	// Portamento
	patch.port = 0.005;
	// Ring modulation
	patch.ringmod.waveform[0] = 0.7;
	patch.ringmod.waveform[1] = 0.8;
	patch.ringmod.waveform[2] = 0.9;
	patch.ringmod.waveform[3] = 1.0;
	patch.ringmod.waveform[4] = 1.0;
	patch.ringmod.waveform[5] = 0.9;
	patch.ringmod.waveform[6] = 0.8;
	patch.ringmod.waveform[7] = 0.7;
	patch.ringmod.detune = 0.0;
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
