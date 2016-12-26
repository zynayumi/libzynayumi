/****************************************************************************

    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    voice.cpp

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

#include "voice.hpp"
#include "engine.hpp"

using namespace zynayumi;

Voice::Voice(Engine& engine,
             const Patch& pa, unsigned char pi, unsigned char vel) :
	pitch(pi), velocity(vel), note_on(true), _engine(engine), _patch(pa) {

	// Tone
	// TODO: support positive time
	bool t_off = _patch.tone.time == 0;
	ayumi_set_tone(&_engine.ay, 0, (int)_engine.pitch2period(pitch));

	// Noise
	bool n_off = _patch.noise.time == 0;

	// Env
	ayumi_set_volume(&_engine.ay, 0, velocity / 8);

	// Ayumi mixer
	ayumi_set_mixer(&_engine.ay, 0, t_off, n_off, 0);
}

void Voice::set_note_off() {
	note_on = false;
	ayumi_set_volume(&_engine.ay, 0, 0);
}

void Voice::update() {
	// TODO
}
