/****************************************************************************

    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    patch.cpp

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

#include "patch.hpp"

namespace zynayumi {

Tone::Tone() : sync(true),
               phase(0.0),
               time(-1),
               detune(0),
               spread(0),
               legacy_tuning(false) {}
	
Noise::Noise() : time(0), period(1) {}

NoisePeriodEnv::NoisePeriodEnv() : attack(1), time(0) {}

Env::Env() : attack_time(0),
             hold1_level(MAX_LEVEL), inter1_time(0),
             hold2_level(MAX_LEVEL), inter2_time(0),
             hold3_level(MAX_LEVEL), decay_time(0),
             sustain_level(MAX_LEVEL), release(0) {}

PitchEnv::PitchEnv() : attack_pitch(0), time(0) {}

Seq::State::State() : tone_pitch(0),
                      noise_period(0),
                      ringmod_depth(1.0),
                      level(MAX_LEVEL) {}

Seq::Seq() : states(Seq::size),
             tempo(120), host_sync(1), freq(18.0),
             loop(0), end(0) {}

RingMod::RingMod() : waveform{MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL},
                     sync(true), phase(0.0), mirror(true), detune(0.0),
                     fixed_freq(1.0), fixed_vs_relative(1.0),
                     depth(MAX_LEVEL) {}

Buzzer::Buzzer() : shape(Buzzer::Shape::DownSaw),
                   sync(true),
                   phase(0.0),
                   time(0.0),
                   detune(0.0) {}

LFO::LFO() : shape(LFO::Shape::Sine),
             freq(1), delay(0), depth(0) {}

Portamento::Portamento() : time(0.0), smoothness(0.5) {}

Pan::Pan() : ym_channel{0.5, 0.25, 0.75} {}

Control::Control() : pitchwheel(2),
                     velocity_sensitivity(0.5),
                     modulation_sensitivity(0.5) {}

Patch::Patch() : emulmode(EmulMode::YM2149),
                 playmode(PlayMode::Mono),
                 gain(1) {}

std::string to_string(PlayMode pm)
{
	switch(pm) {
	case PlayMode::Mono:
		return "Mono";
	case PlayMode::MonoUpArp:
		return "MonoUpArp";
	case PlayMode::MonoDownArp:
		return "MonoDownArp";
	case PlayMode::MonoRandArp:
		return "MonoRandArp";
	case PlayMode::Poly:
		return "Poly";
	case PlayMode::Unison:
		return "Unison";
	case PlayMode::UnisonUpArp:
		return "UnisonUpArp";
	case PlayMode::UnisonDownArp:
		return "UnisonDownArp";
	case PlayMode::UnisonRandArp:
		return "UnisonRandArp";
	default:
		return "";
	}
}

std::string to_string(EmulMode em)
{
	switch(em) {
	case EmulMode::YM2149:
		return "YM2149";
	case EmulMode::AY8910:
		return "AY-3-8910";
	default:
		return "";
	}
}

std::string to_string(Buzzer::Shape sh)
{
	switch(sh) {
	case Buzzer::Shape::DownSaw:
		return "DownSaw";
	case Buzzer::Shape::DownTriangle:
		return "DownTriangle";
	case Buzzer::Shape::UpSaw:
		return "UpSaw";
	case Buzzer::Shape::UpTriangle:
		return "UpTriangle";
	default:
		return "";
	}
}

std::string to_string(LFO::Shape sh)
{
	switch(sh) {
	case LFO::Shape::Sine:
		return "Sine";
	case LFO::Shape::Triangle:
		return "Triangle";
	case LFO::Shape::DownSaw:
		return "DownSaw";
	case LFO::Shape::UpSaw:
		return "UpSaw";
	case LFO::Shape::Square:
		return "Square";
	case LFO::Shape::Rand:
		return "Rand";
	default:
		return "";
	}
}
	
} // ~namespace zynayumi
