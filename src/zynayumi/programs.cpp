/****************************************************************************
    
    Programs for Zynayumi

    programs.cpp

    Copyleft (c) 2020 Nil Geisweiller <ngeiswei@gmail.com>
 
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

#include "programs.hpp"
#include "parameters.hpp"
#include "patch.hpp"

namespace zynayumi {

Programs::Programs()
	: names(count)
	, patches(count)
	// , parameters_seq(count)
{
	//Program 1
	names[0] = "Program-1";

	// NEXT
	// *dynamic_cast<EnumParameter<EmulMode>*>(parameters_seq[0].parameters[EMUL_MODE]) = EmulMode::YM2149;

	// Program 2
	names[0] = "Program-2";
	// *dynamic_cast<EnumParameter<EmulMode>*>(parameters_seq[0].parameters[EMUL_MODE]) = EmulMode::AY8910;
}

// Just to remember it

// // Power bass
// Patch patch;
// patch.name = "Power bass";
// patch.env.inter2_time = 5.0;
// patch.env.sustain_level = 0.5;
// patch.env.release = 0.05;
// patch.pitchenv.attack_pitch = 24.0;
// patch.pitchenv.time = 0.1;
// patch.ringmod.waveform[1] = 0.7;
// patch.ringmod.waveform[5] = 0.5;
// patch.ringmod.waveform[6] = 0.3;
// patch.ringmod.waveform[7] = 0.0;
// patch.ringmod.detune = 0.05;
// patch.lfo.freq = 0.4;
// patch.lfo.delay = 10.0;
// patch.lfo.depth = 0.2;

// TODO: Space Snare
//
// Play mode: Mono
// Tone time: 0.020619
// Tone detune: 0.000000
// Tone transpose: 0
// Noise time: 5.000000
// Noise period: 31
// NoisePeriodEnv attack: 2
// NoisePeriodEnv time: 0.927835
// AmpEnv attack level: 1.000000
// AmpEnv inter1_time: 0.154639
// AmpEnv hold1_level: 0.304124
// AmpEnv inter2_time: 0.618557
// AmpEnv hold2_level: 0.567010
// AmpEnv decay_time: 1.417526
// AmpEnv sustain level: 0.000000
// AmpEnv release: 0.128866
// PitchEnv attack pitch: 0.000000
// PitchEnv time: 0.000000
// Arp pitch1: 0.000000
// Arp pitch2: 0.000000
// Arp pitch3: 0.000000
// Arp freq: 12.500000
// Arp repeat: 0
// RingMod waveform level1: 1.000000
// RingMod waveform level2: 1.000000
// RingMod waveform level3: 1.000000
// RingMod waveform level4: 1.000000
// RingMod waveform level5: 1.000000
// RingMod waveform level6: 1.000000
// RingMod waveform level7: 1.000000
// RingMod waveform level8: 1.000000
// RingMod mirror: 1
// RingMod sync: 1
// RingMod detune: 0.000000
// RingMod transpose: 0
// LFO freq: 1.000000
// LFO delay: 0.000000
// LFO depth: 0.000000
// Portamento: 0.000000
// Pan channel0: 0.500000
// Pan channel1: 0.250000
// Pan channel2: 0.750000
// Pitch wheel: 2
// Emulation mode: YM2149

} // ~namespace zynayumi
