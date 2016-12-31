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
	_note_pitch(pi), _env_smp_count(0), _smp_count(0),
	_ringmod_smp_count(0), _ringmod_waveform_index(0) {
}

void Voice::set_note_off() {
	note_on = false;
	_env_smp_count = 0;
	_actual_sustain_level = env_level;
}

void Voice::update() {
	// Update _time
	_time = _engine.smp2sec(_smp_count);

	// Update tone and noise
	update_tone();
	update_noise();
	ayumi_set_mixer(&_engine.ay, 0, _t_off, _n_off, 0);

	// Update pitch
	update_pitchenv();
	update_port();
	update_lfo();
	update_arp();
	calculate_final_pitch();
	ayumi_set_tone(&_engine.ay, 0, _engine.pitch2period_ym(_final_pitch));

	// Update level
	update_ampenv();
	update_ring();
	ayumi_set_volume(&_engine.ay, 0, (int)(_final_level * 15));

	// Increment sample count since voice on
	_smp_count++;
}

double Voice::linear_interpolate(double x1, double y1, double x2, double y2,
                                 double x) const {
	// assert(x1 <= x and x <= x2);
	double a = (y2 - y1) / (x2 - x1);
	double b = y1;
	return a * (x - x1) + b;
}

void Voice::update_tone() {
	_t_off = 0 <= _patch.tone.time ? _patch.tone.time < _time : false;
}

void Voice::update_noise() {
	_n_off = 0 <= _patch.noise.time ? _patch.noise.time < _time : false;
}

void Voice::update_pitchenv() {
	double apitch = _patch.pitchenv.attack_pitch;
	double ptime = _patch.pitchenv.time;
	_relative_pitchenv_pitch = _patch.pitchenv.time < _time ? 0.0
		: linear_interpolate(0, apitch, ptime, 0.0, _time);
}

void Voice::update_port() {
	_relative_port_pitch =
		(0 <= _engine.previous_pitch and _time < _patch.port ?
		 linear_interpolate(0, _engine.previous_pitch - _note_pitch,
		                    _patch.port, 0, _time)
		 : 0.0);
	_engine.last_pitch = _relative_port_pitch + _note_pitch;
}

void Voice::calculate_final_pitch() {
	_final_pitch = _note_pitch
		+ _patch.tone.detune
		+ _relative_pitchenv_pitch
		+ _relative_port_pitch
		+ _relative_lfo_pitch
		+ _relative_arp_pitch;
}

void Voice::update_lfo() {
	double depth = _patch.lfo.delay < _time ? _patch.lfo.depth
		: linear_interpolate(0, 0, _patch.lfo.delay, _patch.lfo.depth, _time);
	_relative_lfo_pitch = depth * sin(2*M_PI*_time*_patch.lfo.freq);
}

void Voice::update_arp()
{
	// Find the pitch index
	auto count2index = [&]() -> size_t {
		size_t index = _smp_count * _patch.arp.freq / _engine.sample_rate;
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
		_relative_arp_pitch =
			1 < _engine.pitches.size() ? count2pitch(false) - _note_pitch : 0.0;
		break;
	case PlayMode::DownArp:
		_relative_arp_pitch =
			1 < _engine.pitches.size() ? count2pitch(true) - _note_pitch : 0.0;
		break;
	default:
		std::cerr << "Not implemented" << std::endl;
	}
}

void Voice::update_ampenv() {
	// Determine portion of the envelope to interpolate
	double env_time = _engine.smp2sec(_env_smp_count);
	double x1, y1, x2, y2;
	if (note_on) {
		double t1 = _patch.ampenv.time1;
		double t12 = t1 + _patch.ampenv.time2;
		double t123 = t12 + _patch.ampenv.time3;
		if (env_time <= t1) {
			x1 = 0;
			y1 = _patch.ampenv.attack_level;
			x2 = t1;
			y2 = _patch.ampenv.level1;
		} else if (env_time <= t12) {
			x1 = t1;
			y1 = _patch.ampenv.level1;
			x2 = t12;
			y2 = _patch.ampenv.level2;
		} else if (env_time <= t123) {
			x1 = t12;
			y1 = _patch.ampenv.level2;
			x2 = t123;
			y2 = _patch.ampenv.sustain_level;
		} else {
			x1 = t123;
			y1 = _patch.ampenv.sustain_level;
			x2 = x1 + 1;
			y2 = y1;
		}
	} else {                    // Note off
		if (env_time <= _patch.ampenv.release) {
			x1 = 0;
			y1 = _actual_sustain_level;
			x2 = _patch.ampenv.release;
			y2 = 0;
		} else {
			x1 = _patch.ampenv.release;
			y1 = 0;
			x2 = x1 + 1;
			y2 = 0;
		}
	}

	// Calculate the envelope level
	env_level = linear_interpolate(x1, y1, x2, y2, env_time);

	// Adjust according to key velocity
	env_level *= (double)velocity / 127.0;

	// Increment the envelope sample count
	_env_smp_count++;
}

void Voice::update_ring() {
	// Get the waveform value
	double waveform_val = _patch.ringmod.waveform[_ringmod_waveform_index];
	_final_level = waveform_val * env_level;

	// Update _ringmod_smp_count and _ringmod_waveform_index
	double ringmod_pitch = _patch.ringmod.detune + _final_pitch;
	double waveform_period = 2 * _engine.pitch2period_ym(ringmod_pitch);

	_ringmod_smp_count += _engine.ay.step * DECIMATE_FACTOR;
	// If it goes from very low pitch to very high pitch we migh need
	// to remove waveform_period from _ringmod_smp_count more than
	// once.
	while (waveform_period <= _ringmod_smp_count)
		_ringmod_smp_count -= waveform_period;
	_ringmod_waveform_index = (RING_MOD_WAVEFORM_SIZE * _ringmod_smp_count)
		/ (0.5 * waveform_period);
	if (RING_MOD_WAVEFORM_SIZE <= _ringmod_waveform_index)
		_ringmod_waveform_index =
			(2 * RING_MOD_WAVEFORM_SIZE - 1) - _ringmod_waveform_index;
}
