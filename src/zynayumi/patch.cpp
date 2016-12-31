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

Tone::Tone() : time(-1), detune(0) {}
	
Noise::Noise() : time(0), period(1) {}

AmpEnv::AmpEnv() : attack_level(1),
                   time1(0), level1(1),
                   time2(0), level2(1),
                   time3(0), sustain_level(1), release(0) {}

PitchEnv::PitchEnv() : attack_pitch(0),
                       time(0) {}

Arp::Arp() : pitch1(0), pitch2(0), pitch3(0), freq(1), repeat(0) {}

RingMod::RingMod() : waveform{1.0, 1.0, 1.0, 1.0}, detune(0) {}

LFO::LFO() : freq(1), delay(0), depth(0) {}

Patch::Patch() : playmode(PlayMode::Legato), port(0) {}

} // ~namespace zynayumi
