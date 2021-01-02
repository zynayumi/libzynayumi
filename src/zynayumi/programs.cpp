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

Programs::Programs(Zynayumi& zynayumi)
{
	// Memory allocate programs
	for (unsigned i = 0; i < count; i++)
		parameters_pts[i] = new Parameters(zynayumi, patches[i]);

	// Initialize programs
	Parameters* prmtrs = nullptr;

	//Program 0
	prmtrs = parameters_pts[0];
	prmtrs->patch.name = "Heu Bass";
	prmtrs->patch.emulmode = zynayumi::EmulMode::YM2149;
	prmtrs->patch.playmode = zynayumi::PlayMode::Mono;
	prmtrs->patch.tone.reset = true;
	prmtrs->patch.tone.phase = 0;
	prmtrs->patch.tone.time = std::numeric_limits<float>::infinity();
	prmtrs->tone_detune = 0;
	prmtrs->tone_transpose = -12;
	prmtrs->patch.tone.spread = 0;
	prmtrs->patch.tone.legacy_tuning = false;
	prmtrs->patch.noise.time = 0.000000;
	prmtrs->patch.noise.period = 16;
	prmtrs->patch.noise_period_env.attack = 1;
	prmtrs->patch.noise_period_env.time = 0;
	prmtrs->patch.env.attack_time = 0;
	prmtrs->patch.env.hold1_level = 15;
	prmtrs->patch.env.inter1_time = 0;
	prmtrs->patch.env.hold2_level = 15;
	prmtrs->patch.env.inter2_time = 0;
	prmtrs->patch.env.hold3_level = 15;
	prmtrs->patch.env.decay_time = 5;
	prmtrs->patch.env.sustain_level = 8;
	prmtrs->patch.env.release = 0.05;
	prmtrs->patch.pitchenv.attack_pitch = 24;
	prmtrs->patch.pitchenv.time = 0.04;
	prmtrs->patch.ringmod.waveform[0] = 15;
	prmtrs->patch.ringmod.waveform[1] = 14;
	prmtrs->patch.ringmod.waveform[2] = 15;
	prmtrs->patch.ringmod.waveform[3] = 15;
	prmtrs->patch.ringmod.waveform[4] = 13;
	prmtrs->patch.ringmod.waveform[5] = 15;
	prmtrs->patch.ringmod.waveform[6] = 15;
	prmtrs->patch.ringmod.waveform[7] = 12;
	prmtrs->patch.ringmod.waveform[8] = 15;
	prmtrs->patch.ringmod.waveform[9] = 15;
	prmtrs->patch.ringmod.waveform[10] = 11;
	prmtrs->patch.ringmod.waveform[11] = 15;
	prmtrs->patch.ringmod.waveform[12] = 15;
	prmtrs->patch.ringmod.waveform[13] = 10;
	prmtrs->patch.ringmod.waveform[14] = 15;
	prmtrs->patch.ringmod.waveform[15] = 15;
	prmtrs->patch.ringmod.reset = true;
	prmtrs->patch.ringmod.phase = 0;
	prmtrs->patch.ringmod.mirror = true;
	prmtrs->ringmod_detune = 0;
	prmtrs->ringmod_transpose = 0;
	prmtrs->patch.ringmod.fixed_freq = 1;
	prmtrs->patch.ringmod.fixed_vs_relative = 1;
	prmtrs->patch.ringmod.depth = 15;
	prmtrs->patch.buzzer.shape = zynayumi::Buzzer::Shape::DownSaw;
	prmtrs->patch.buzzer.reset = true;
	prmtrs->patch.buzzer.phase = 0;
	prmtrs->patch.buzzer.time = 0.000000;
	prmtrs->buzzer_detune = 0;
	prmtrs->buzzer_transpose = 0;
	prmtrs->patch.seq.states[0].tone_pitch = 0;
	prmtrs->patch.seq.states[0].noise_period = 0;
	prmtrs->patch.seq.states[0].ringmod_depth = 15;
	prmtrs->patch.seq.states[0].level = 15;
	prmtrs->patch.seq.states[1].tone_pitch = 0;
	prmtrs->patch.seq.states[1].noise_period = 0;
	prmtrs->patch.seq.states[1].ringmod_depth = 15;
	prmtrs->patch.seq.states[1].level = 15;
	prmtrs->patch.seq.states[2].tone_pitch = 0;
	prmtrs->patch.seq.states[2].noise_period = 0;
	prmtrs->patch.seq.states[2].ringmod_depth = 15;
	prmtrs->patch.seq.states[2].level = 15;
	prmtrs->patch.seq.states[3].tone_pitch = 0;
	prmtrs->patch.seq.states[3].noise_period = 0;
	prmtrs->patch.seq.states[3].ringmod_depth = 15;
	prmtrs->patch.seq.states[3].level = 15;
	prmtrs->patch.seq.states[4].tone_pitch = 0;
	prmtrs->patch.seq.states[4].noise_period = 0;
	prmtrs->patch.seq.states[4].ringmod_depth = 15;
	prmtrs->patch.seq.states[4].level = 15;
	prmtrs->patch.seq.states[5].tone_pitch = 0;
	prmtrs->patch.seq.states[5].noise_period = 0;
	prmtrs->patch.seq.states[5].ringmod_depth = 15;
	prmtrs->patch.seq.states[5].level = 15;
	prmtrs->patch.seq.states[6].tone_pitch = 0;
	prmtrs->patch.seq.states[6].noise_period = 0;
	prmtrs->patch.seq.states[6].ringmod_depth = 15;
	prmtrs->patch.seq.states[6].level = 15;
	prmtrs->patch.seq.states[7].tone_pitch = 0;
	prmtrs->patch.seq.states[7].noise_period = 0;
	prmtrs->patch.seq.states[7].ringmod_depth = 15;
	prmtrs->patch.seq.states[7].level = 15;
	prmtrs->patch.seq.states[8].tone_pitch = 0;
	prmtrs->patch.seq.states[8].noise_period = 0;
	prmtrs->patch.seq.states[8].ringmod_depth = 15;
	prmtrs->patch.seq.states[8].level = 15;
	prmtrs->patch.seq.states[9].tone_pitch = 0;
	prmtrs->patch.seq.states[9].noise_period = 0;
	prmtrs->patch.seq.states[9].ringmod_depth = 15;
	prmtrs->patch.seq.states[9].level = 15;
	prmtrs->patch.seq.states[10].tone_pitch = 0;
	prmtrs->patch.seq.states[10].noise_period = 0;
	prmtrs->patch.seq.states[10].ringmod_depth = 15;
	prmtrs->patch.seq.states[10].level = 15;
	prmtrs->patch.seq.states[11].tone_pitch = 0;
	prmtrs->patch.seq.states[11].noise_period = 0;
	prmtrs->patch.seq.states[11].ringmod_depth = 15;
	prmtrs->patch.seq.states[11].level = 15;
	prmtrs->patch.seq.states[12].tone_pitch = 0;
	prmtrs->patch.seq.states[12].noise_period = 0;
	prmtrs->patch.seq.states[12].ringmod_depth = 15;
	prmtrs->patch.seq.states[12].level = 15;
	prmtrs->patch.seq.states[13].tone_pitch = 0;
	prmtrs->patch.seq.states[13].noise_period = 0;
	prmtrs->patch.seq.states[13].ringmod_depth = 15;
	prmtrs->patch.seq.states[13].level = 15;
	prmtrs->patch.seq.states[14].tone_pitch = 0;
	prmtrs->patch.seq.states[14].noise_period = 0;
	prmtrs->patch.seq.states[14].ringmod_depth = 15;
	prmtrs->patch.seq.states[14].level = 15;
	prmtrs->patch.seq.states[15].tone_pitch = 0;
	prmtrs->patch.seq.states[15].noise_period = 0;
	prmtrs->patch.seq.states[15].ringmod_depth = 15;
	prmtrs->patch.seq.states[15].level = 15;
	prmtrs->patch.seq.tempo = 120;
	prmtrs->patch.seq.host_sync = true;
	prmtrs->seq_beat_divisor = 9;
	prmtrs->seq_beat_multiplier = 1;
	prmtrs->patch.seq.loop = 0;
	prmtrs->patch.seq.end = 0;
	prmtrs->patch.lfo.shape = zynayumi::LFO::Shape::Triangle;
	prmtrs->patch.lfo.freq = 4;
	prmtrs->patch.lfo.delay = 10;
	prmtrs->patch.lfo.depth = 1;
	prmtrs->patch.portamento.time = 0;
	prmtrs->patch.portamento.smoothness = 0.5;
	prmtrs->patch.gain = 1;
	prmtrs->patch.pan.ym_channel[0] = 0.5;
	prmtrs->patch.pan.ym_channel[1] = 0.25;
	prmtrs->patch.pan.ym_channel[2] = 0.75;
	prmtrs->patch.control.pitchwheel = 2;
	prmtrs->patch.control.velocity_sensitivity = 0.5;
	prmtrs->patch.control.ringmod_velocity_sensitivity = 0;
	prmtrs->patch.control.noise_period_pitch_sensitivity = 0;
	prmtrs->patch.control.modulation_sensitivity = 0.5;

	// NEXT
}

Programs::~Programs()
{
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
// RingMod reset: 1
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
