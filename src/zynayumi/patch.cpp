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

#include <limits>

namespace zynayumi {

Tone::Tone() : reset(true),
               phase(0.0),
               time(std::numeric_limits<float>::infinity()),
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

PitchEnv::PitchEnv() : attack_pitch(0), time(0), smoothness(0.5) {}

RingMod::RingMod() : waveform{MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL, MAX_LEVEL},
                     reset(true), sync(false), phase(0.0),
							loop(RingMod::Loop::PingPong), detune(0.0),
                     fixed_pitch(0.0), fixed_vs_relative(1.0),
                     depth(MAX_LEVEL) {}

Buzzer::Buzzer() : enabled(false),
                   shape(Buzzer::Shape::DownSaw) {}

Seq::State::State() : tone_pitch(0),
                      noise_period(0),
                      ringmod_pitch(0),
                      ringmod_depth(MAX_LEVEL),
                      level(MAX_LEVEL),
                      tone_on(true),
                      noise_on(true)
{}

Seq::Seq() : states(Seq::size),
             tempo(120), host_sync(1), freq(18.0),
             loop(0), end(0) {}

LFO::LFO() : shape(LFO::Shape::Sine),
             freq(1), delay(0), depth(0) {}

Portamento::Portamento() : time(0.0), smoothness(0.5) {}

Mixer::Mixer() : enabled{true, true, true},
                 pan{0.5, 0.25, 0.75},
                 gain(1.0) {}

Control::Control() : pitchwheel(2),
                     velocity_sensitivity(0.5),
                     modulation_sensitivity(0.5),
                     midi_ch{Control::MidiChannel::Any,
                             Control::MidiChannel::Any,
                             Control::MidiChannel::Any} {}

Patch::Patch() : emulmode(EmulMode::YM2149),
                 playmode(PlayMode::Mono) {}

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
	case PlayMode::Unison:
		return "Unison";
	case PlayMode::UnisonUpArp:
		return "UnisonUpArp";
	case PlayMode::UnisonDownArp:
		return "UnisonDownArp";
	case PlayMode::UnisonRandArp:
		return "UnisonRandArp";
	case PlayMode::Poly:
		return "Poly";
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

std::string to_string(RingMod::Loop lp)
{
	switch(lp) {
	case RingMod::Loop::Off:
		return "Off";
	case RingMod::Loop::Forward:
		return "Forward";
	case RingMod::Loop::PingPong:
		return "PingPong";
	default:
		return "";
	}
}

std::string to_string(Buzzer::Shape sh)
{
	switch(sh) {
	case Buzzer::Shape::DownSaw:
		return "DownSaw";
	case Buzzer::Shape::UpSaw:
		return "UpSaw";
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

std::string to_string(Control::MidiChannel mc)
{
	switch(mc) {
	case Control::MidiChannel::Any:
		return "Any";
	case Control::MidiChannel::c1:
		return "1";
	case Control::MidiChannel::c2:
		return "2";
	case Control::MidiChannel::c3:
		return "3";
	case Control::MidiChannel::c4:
		return "4";
	case Control::MidiChannel::c5:
		return "5";
	case Control::MidiChannel::c6:
		return "6";
	case Control::MidiChannel::c7:
		return "7";
	case Control::MidiChannel::c8:
		return "8";
	case Control::MidiChannel::c9:
		return "9";
	case Control::MidiChannel::c10:
		return "10";
	case Control::MidiChannel::c11:
		return "11";
	case Control::MidiChannel::c12:
		return "12";
	case Control::MidiChannel::c13:
		return "13";
	case Control::MidiChannel::c14:
		return "14";
	case Control::MidiChannel::c15:
		return "15";
	case Control::MidiChannel::c16:
		return "16";
	default:
		return "";
	}
}

} // ~namespace zynayumi
