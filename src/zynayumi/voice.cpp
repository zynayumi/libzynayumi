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

#include <iostream>
#include <cmath>

#include "voice.hpp"
#include "engine.hpp"

using namespace zynayumi;

Voice::Voice(Engine& engine,
             const Patch& pa, unsigned char pi, unsigned char vel) :
	pitch(pi), velocity(vel), note_on(true), _engine(engine), _patch(pa),
	_pitch(pi), _fine_pitch(pi), _env_smp_count(0), _smp_count(0) {

	// Tone
	// TODO: support positive time
	bool t_off = _patch.tone.time == 0;
	ayumi_set_tone(&_engine.ay, 0, (int)_engine.pitch2period_ym(_fine_pitch));

	// Noise
	bool n_off = _patch.noise.time == 0;

	// Ayumi mixer
	ayumi_set_mixer(&_engine.ay, 0, t_off, n_off, 0);
}

void Voice::set_note_off() {
	note_on = false;
	_env_smp_count = 0;
	_actual_sustain_level = env_level;
}

void Voice::update() {
	update_env_level();
	update_arp();
	update_lfo();
	update_port();
	update_ring();

	// Increment sample count
	_smp_count++;
}

float Voice::linear_interpolate(float x1, float y1, float x2, float y2,
                                float x) const {
	// assert(x1 <= x and x <= x2);
	float a = (y2 - y1) / (x2 - x1);
	float b = y1;
	return a * (x - x1) + b;
}

void Voice::update_env_level() {
	// Determine portion of the envelope to interpolate
	float env_time = _engine.smp2sec(_env_smp_count);
	float x1, y1, x2, y2;
	if (note_on) {
		float t1 = _patch.env.time1;
		float t12 = t1 + _patch.env.time2;
		float t123 = t12 + _patch.env.time3;
		if (env_time <= t1) {
			x1 = 0;
			y1 = _patch.env.attack_level;
			x2 = t1;
			y2 = _patch.env.level1;
		} else if (env_time <= t12) {
			x1 = t1;
			y1 = _patch.env.level1;
			x2 = t12;
			y2 = _patch.env.level2;
		} else if (env_time <= t123) {
			x1 = t12;
			y1 = _patch.env.level2;
			x2 = t123;
			y2 = _patch.env.sustain_level;
		} else {
			x1 = t123;
			y1 = _patch.env.sustain_level;
			x2 = x1 + 1;
			y2 = y1;
		}
	} else {                    // Note off
		if (env_time <= _patch.env.release) {
			x1 = 0;
			y1 = _actual_sustain_level;
			x2 = _patch.env.release;
			y2 = 0;
		} else {
			x1 = _patch.env.release;
			y1 = 0;
			x2 = x1 + 1;
			y2 = 0;
		}
	}

	// Calculate the envelope level
	env_level = linear_interpolate(x1, y1, x2, y2, env_time);

	// Adjust according to key velocity
	env_level *= (float)velocity / 127.0;

	// Update the ayumi volume
	ayumi_set_volume(&_engine.ay, 0, (int)(env_level * 15));

	// Increment the envelope sample count
	_env_smp_count++;
}

void Voice::update_arp()
{
	// Find the pitch index
	auto count2index = [&]() -> size_t {
		size_t index = _smp_count / (_engine.sample_rate / _patch.arp.freq);
		index %= _engine.pitches.size();
		return index;
	};
	// Find the pitch
	auto count2pitch = [&](bool down) -> unsigned char {
		size_t index = count2index();
		if (down)
			index = (_engine.pitches.size() - 1) - index;
		return *std::next(_engine.pitches.begin(), index);
	};

	switch (_patch.playmode) {
	case PlayMode::Legato:
		break;
	case PlayMode::UpArp:
		if (1 < _engine.pitches.size()) {
			_pitch = count2pitch(false);
			ayumi_set_tone(&_engine.ay, 0, (int)_engine.pitch2period_ym(_pitch));
		}
		break;
	case PlayMode::DownArp:
		if (1 < _engine.pitches.size()) {
			_pitch = count2pitch(true);
			ayumi_set_tone(&_engine.ay, 0, (int)_engine.pitch2period_ym(_pitch));
		}
		break;
	default:
		std::cerr << "Not implemented" << std::endl;
	}
}

void Voice::update_lfo() {
	float time = _engine.smp2sec(_smp_count);
	float depth = _patch.lfo.delay < time ? _patch.lfo.depth
		: linear_interpolate(0, 0, _patch.lfo.delay, _patch.lfo.depth, time);
	float relative_fine_pitch = depth * sin(2*M_PI*time*_patch.lfo.freq);
	_fine_pitch = _pitch + relative_fine_pitch;
	ayumi_set_tone(&_engine.ay, 0, (int)_engine.pitch2period_ym(_fine_pitch));
}

void Voice::update_port() {
	float time = _engine.smp2sec(_smp_count);
	if (0 <= _engine.previous_pitch and time < _patch.port) {
		_port_relative_pitch =
			linear_interpolate(0, _engine.previous_pitch - pitch,
			                   _patch.port, 0, time);
		float _port_fine_pitch = _fine_pitch + _port_relative_pitch;
		ayumi_set_tone(&_engine.ay, 0,
		               (int)_engine.pitch2period_ym(_port_fine_pitch));

		_engine.last_pitch = _port_relative_pitch + pitch;
	}
}

void Voice::update_ring() {
	// Get the waveform value
	float waveform_val = _patch.ringmod.waveform[_ringmod_waveform_index];
	level = waveform_val * env_level;

	// Update the ayumi volume
	ayumi_set_volume(&_engine.ay, 0, (int)(level * 15));

	// Update _ringmod_smp_count and _ringmod_waveform_index
	float ringmod_fine_pitch = _patch.ringmod.detune + _fine_pitch;
	unsigned waveform_period = _engine.pitch2period(ringmod_fine_pitch);
    if (waveform_period <= ++_ringmod_smp_count) {
	    _ringmod_smp_count = 0;
	}
    _ringmod_waveform_index =
	    (float)(RING_MOD_WAVEFORM_SIZE * _ringmod_smp_count)
	    / (float)waveform_period;
}
