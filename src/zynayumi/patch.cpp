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

Tone::Tone() : time(-1), detune(0), spread(0) {}
	
Noise::Noise() : time(0), period(1) {}

NoisePeriodEnv::NoisePeriodEnv() : attack(1), time(0) {}

AmpEnv::AmpEnv() : attack_time(0),
                   hold1_level(1), inter1_time(0),
                   hold2_level(1), inter2_time(0),
                   hold3_level(1), decay_time(0),
                   sustain_level(1), release(0) {}

PitchEnv::PitchEnv() : attack_pitch(0), time(0) {}

Arp::Arp() : pitch1(0), pitch2(0), pitch3(0), freq(12.5), repeat(0) {}

RingMod::RingMod() : waveform{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
                     mirror(true), sync(true), detune(0) {}

LFO::LFO() : freq(1), delay(0), depth(0) {}

Pan::Pan() : ym_channel{0.5, 0.25, 0.75} {}

Control::Control() : pitchwheel(2),
                     velocity_sensitivity(0.5),
                     modulation_sensitivity(0.5)
{}

Patch::Patch() : emulmode(EmulMode::YM2149),
                 playmode(PlayMode::Mono),
                 port(0), gain(1)
{}

std::string to_string(PlayMode pm)
{
	switch(pm) {
	case PlayMode::Mono:
		return "Mono";
	case PlayMode::MonoUpArp:
		return "MonoUpArp";
	case PlayMode::MonoDownArp:
		return "MonoDownArp";
	case PlayMode::MonoRndArp:
		return "MonoRndArp";
	case PlayMode::Poly:
		return "Poly";
	case PlayMode::Unison:
		return "Unison";
	case PlayMode::UnisonUpArp:
		return "UnisonUpArp";
	case PlayMode::UnisonDownArp:
		return "UnisonDownArp";
	case PlayMode::UnisonRndArp:
		return "UnisonRndArp";
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

} // ~namespace zynayumi
