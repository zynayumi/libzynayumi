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
#include <random>
#include <algorithm>

#include "voice.hpp"
#include "engine.hpp"

using namespace zynayumi;

Voice::Voice(Engine& engine, const Patch& pa,
             unsigned char ych, unsigned char pi, unsigned char vel)
	: ym_channel(ych)
	, pitch(pi)
	, velocity(vel)
	, velocity_level(velocity_to_level(pa.control.velocity_sensitivity, velocity))
	, note_on(true)
	, _engine(&engine)
	, _patch(&pa)
	, _initial_pitch(pi)
	, _seq_step(-1)
	, _seq_change(true)
	, _seq_index(0)
	, _seq_level(1.0)
	, _relative_seq_pitch(0)
	, _seq_rnd_offset_step(rand())
	, _rnd_index(-1)
	, _env_smp_count(0)
	, _on_smp_count(0)
	, _pitch_smp_count(0)
	, _ringmod_smp_count(0)
	, _ringmod_back(false)
	, _ringmod_waveform_index(0)
	, _first_update(true)
	, _tone_trigger(false)
	, _last_tone(_engine->ay.channels[ym_channel].tone)
{
}

void Voice::set_note_off()
{
	note_on = false;
	_env_smp_count = 0;
	_actual_sustain_level = env_level;
}

void Voice::update()
{
	// Update time
	on_time = _engine->smp2sec(_on_smp_count);
	pitch_time = _engine->smp2sec(_pitch_smp_count);

	// Update pan
	update_pan();

	// Update seq
	update_seq();

	// Update tone and noise
	update_tone_off();
	update_noise_off();
	update_buzzer_off();
	update_noise_period();
	ayumi_set_noise(&_engine->ay, _noise_period);
	ayumi_set_mixer(&_engine->ay, ym_channel, _tone_off, _noise_off, !_buzzer_off);

	// Update pitch
	update_pitchenv();
	update_portamento();
	update_lfo();
	update_arp();
	update_final_pitch();
	update_tone();

	// Reset
	if (_first_update) {
		reset_tone();
		reset_ringmod();
		reset_buzzer();
		_first_update = false;
		if (_patch->tone.reset and not _tone_off)
			_tone_trigger = true;
	}

	// Sync
	if (_patch->ringmod.sync) {
		// Update tone trigger
		struct tone_channel& ch = _engine->ay.channels[ym_channel];
		if (_last_tone != ch.tone) {
			_tone_trigger = true;//(ch.tone == 1);
			_last_tone = ch.tone;
		}

		// Sync upon tone trigger
		if (_tone_trigger) {
			if (_buzzer_off) {
				sync_ringmod();
			} else {
				sync_buzzer();
			}
			_tone_trigger = false;
		}
	}

	// Update buzzer
	update_buzzer();

	// Update level, including ring modulation
	update_env();
	update_ringmod();
	update_seq_level();
	update_final_level();
	ayumi_set_volume(&_engine->ay, ym_channel, std::lround(_final_level * MAX_LEVEL));

	// Increment sample count since voice on or pitch change
	_on_smp_count++;
	_pitch_smp_count++;
}

void Voice::set_note_pitch(unsigned char pi)
{
	pitch = pi;
	_initial_pitch = pi;
	_pitch_smp_count = 0;
}

double Voice::linear_interpolate(double x1, double y1,
                                 double x2, double y2,
                                 double x)
{
	if (0 != (x2 - x1)) {
		double a = (y2 - y1) / (x2 - x1);
		double b = y1;
		return a * (x - x1) + b;
	} else {
		return (y2 - y1) / 2.0;
	}
}

double Voice::logistic_interpolate(double x1, double y1,
                                   double x2, double y2,
                                   double x, double scale)
{
	// mu is the x-axis coordinate of the inflection point
	double mu = (x2 - x1) / 2.0;

	// We resolve the following equations, by adding and subtracting
	//
	// y1 = b + a / (1.0 + exp(-(x1 - mu) / scale))
	// y2 = b + a / (1.0 + exp(-(x2 - mu) / scale))
	//
	// or alternatively by using maxima with the command
	//
	// solve([y1 = b + a / (1.0 + exp(-(x1 - mu) / scale)), y2 = b + a / (1.0 + exp(-(x2 - mu) / scale))], [a, b]);
	//
	// to obtain
	//
	// a = - (e1 * (-e2 - 1) * y2 + (-e2 - 1) * y2 + (e1 * (e2 + 1) + e2 + 1) * y1) / (e1 - e2)
	// b = ((-e2 - 1) * y2 + (e1 + 1) * y1) / (e1 - e2)
	//
	// with
	//
	// e1 = exp((mu - x1) / scale)
	// e2 = exp((mu - x2) / scale)
	double e1 = exp((mu - x1) / scale);
	double e2 = exp((mu - x2) / scale);
	double a = - (e1 * (-e2 - 1) * y2 + (-e2 - 1) * y2 + (e1 * (e2 + 1) + e2 + 1) * y1) / (e1 - e2);
	double b = ((-e2 - 1) * y2 + (e1 + 1) * y1) / (e1 - e2);

	return b + a / (1.0 + exp(-(x - mu) / scale));
}

double Voice::ym_channel_to_spread() const
{
	switch(ym_channel) {
	case 0: return 0.0;
	case 1: return -_patch->tone.spread;
	case 2: return _patch->tone.spread;
	default: return 0.0;
	}
}

void Voice::update_pan()
{
	ayumi_set_pan(&_engine->ay, ym_channel, _patch->pan.ym_channel[ym_channel], 0);
}

void Voice::update_seq()
{
	// Update _seq_step and _seq_index

	int step = _on_smp_count * _patch->seq.freq / _engine->sample_rate;
	_seq_change = _seq_step != step;

	if (!_seq_change)
		return;

	_seq_step = step;

	auto step2index = [&](size_t repeat, size_t size) -> int {
		int index = _seq_step;
		if (size <= index)
			index = repeat + (index % (size - repeat));
		return index;
	};

	if (_patch->seq.loop < _patch->seq.end) {
		_seq_index = step2index(_patch->seq.loop, _patch->seq.end);
	} else {
		if (_seq_step < _patch->seq.end) {
			_seq_index = (int)_seq_step;
		} else {
			_seq_index = -1;
		}
	}
}

void Voice::update_tone()
{
	double tp = _engine->pitch2toneperiod(_final_pitch);
	ayumi_set_tone(&_engine->ay, ym_channel, tp);
}

void Voice::update_tone_off()
{
	_tone_off = 0 <= _patch->tone.time ? _patch->tone.time < on_time : false;
}

void Voice::update_noise_off()
{
	_noise_off = 0 <= _patch->noise.time ? _patch->noise.time < on_time : false;
}

void Voice::update_noise_period()
{
	// Calculate noise period from envelope
	double aperiod = _patch->noise_period_env.attack;
	double envtime = _patch->noise_period_env.time;
	double fperiod = _patch->noise.period;
	_noise_period = envtime < on_time ? fperiod
		: std::round(linear_interpolate(0.0, aperiod, envtime, fperiod, on_time));
	double npps = _patch->control.noise_period_pitch_sensitivity;
	// TODO: optimize by putting in the constructor
	int delta_period = (int)std::round(linear_interpolate(0.0, +31 * npps,
	                                                      127.0, -31 * npps,
	                                                      (double)pitch));
	_noise_period += delta_period;

	// Calculate noise period from sequencer
	if (0 <= _seq_index)
		_noise_period += _patch->seq.states[_seq_index].noise_period;

	// Clamp if out of bound
	_noise_period = std::clamp(_noise_period, 0, 31);
}

void Voice::update_pitchenv()
{
	// NEXT support smoothness
	double apitch = _patch->pitchenv.attack_pitch;
	double ptime = _patch->pitchenv.time;
	_relative_pitchenv_pitch = _patch->pitchenv.time < on_time ? 0.0
		: linear_interpolate(0.0, apitch, ptime, 0.0, on_time);
}

void Voice::update_portamento()
{
	double pitch_diff = _engine->previous_pitch - _initial_pitch;
	double end_time = _patch->portamento.time + _engine->portamento_time;

	if (0.0 < end_time) {
		const double scale_L = 0.05;
		const double scale_U = 400;
		double scale = end_time *
			linear_interpolate(0.0, scale_U, 1.0, scale_L,
			                   _patch->portamento.smoothness);
		_relative_port_pitch =
			(0 != pitch_diff and pitch_time < end_time ?
			 logistic_interpolate(0, pitch_diff, end_time, 0, pitch_time, scale)
			 : 0.0);
		_engine->last_pitch = _relative_port_pitch + _initial_pitch;
	} else {
		_engine->last_pitch = _initial_pitch;
	}

	// Make sure that increasing the portamento time once over doesn't
	// retrigger it
	if (end_time <= pitch_time)
		_engine->previous_pitch = _engine->last_pitch;
}

void Voice::update_lfo()
{
	double depth = _patch->lfo.delay < on_time ? _patch->lfo.depth
		: linear_interpolate(0, 0, _patch->lfo.delay, _patch->lfo.depth, on_time);
	depth += _engine->mw_depth;
	double lfo_pitch;
	switch (_patch->lfo.shape) {
	case LFO::Shape::Sine:
		lfo_pitch = lfo_sine_pitch(_patch->lfo.freq, on_time);
		break;
	case LFO::Shape::Triangle:
		lfo_pitch = lfo_triangle_pitch(_patch->lfo.freq, on_time);
		break;
	case LFO::Shape::DownSaw:
		lfo_pitch = lfo_downsaw_pitch(_patch->lfo.freq, on_time);
		break;
	case LFO::Shape::UpSaw:
		lfo_pitch = lfo_upsaw_pitch(_patch->lfo.freq, on_time);
		break;
	case LFO::Shape::Square:
		lfo_pitch = lfo_square_pitch(_patch->lfo.freq, on_time);
		break;
	case LFO::Shape::Rand:
		lfo_pitch = lfo_rand_pitch(_patch->lfo.freq, on_time);
		break;
	}
	_relative_lfo_pitch = depth * lfo_pitch;
}

void Voice::update_arp()
{
	if (!_seq_change)
		return;

	auto count2index = [&](size_t repeat, size_t size) -> unsigned {
		unsigned index = _seq_step;
		if (size <= index)
			index = repeat + (index % (size - repeat));
		return index;
	};
	// Find the pitch
	auto count2pitch = [&](bool down) -> unsigned char {
		unsigned index = count2index(0, _engine->pitches.size());
		if (down)
			index = (_engine->pitches.size() - 1) - index;
		return *std::next(_engine->pitches.begin(), index);
	};

	// Like the above but return a random index and pitch
	auto count2rndindex = [&](size_t size) -> unsigned {
		if (_seq_change) {
			bool same_index;
			unsigned new_index;
			do {                   // Try the next random index if the
				                    // note wouldn't changed.
				new_index = hash(_seq_rnd_offset_step + _seq_step) % size;
				same_index = new_index == _rnd_index;
				if (same_index)
					++_seq_rnd_offset_step;
			} while	(same_index);
			_rnd_index = new_index;
			return _rnd_index;
		}
		else {
			_rnd_index = hash(_seq_rnd_offset_step + _seq_step) % size;
			return _rnd_index;
		}
	};
	auto count2rndpitch = [&]() -> unsigned char {
		unsigned index = count2rndindex(_engine->pitches.size());
		return *std::next(_engine->pitches.begin(), index);
	};

	// Take care of playmode arp
	switch (_patch->playmode) {
	case PlayMode::MonoUpArp:
	case PlayMode::UnisonUpArp:
		_relative_seq_pitch = 1 < _engine->pitches.size() ?
			count2pitch(false) - _initial_pitch : 0.0;
		break;
	case PlayMode::MonoDownArp:
	case PlayMode::UnisonDownArp:
		_relative_seq_pitch = 1 < _engine->pitches.size() ?
			count2pitch(true) - _initial_pitch : 0.0;
		break;
	case PlayMode::MonoRandArp:
	case PlayMode::UnisonRandArp:
		_relative_seq_pitch = 1 < _engine->pitches.size() ?
			count2rndpitch() - _initial_pitch : 0.0;
		break;
	case PlayMode::Unison:
	case PlayMode::Mono:
	case PlayMode::Poly:
		_relative_seq_pitch = 0;
		break;
	default:
		std::cerr << "Not implemented" << std::endl;
	}

	// Take care of seq arp
	if (0 <= _seq_index)
		_relative_seq_pitch += _patch->seq.states[_seq_index].tone_pitch;
}

void Voice::update_final_pitch()
{
	_final_pitch = _initial_pitch
		+ _patch->tone.detune
		+ ym_channel_to_spread()
		+ _relative_pitchenv_pitch
		+ _relative_port_pitch
		+ _relative_lfo_pitch
		+ _engine->pw_pitch
		+ _relative_seq_pitch;
}

void Voice::update_env()
{
	if (_buzzer_off) {
		// Determine portion of the envelope to interpolate
		double env_time = _engine->smp2sec(_env_smp_count);
		double x1, y1, x2, y2;
		if (note_on) {
			double ta = _patch->env.attack_time;
			double ta1 = ta + _patch->env.inter1_time;
			double ta12 = ta1 + _patch->env.inter2_time;
			double ta123 = ta12 + _patch->env.decay_time;
			if (env_time <= ta) {
				x1 = 0;
				y1 = 0;
				x2 = ta;
				y2 = normalize_level(_patch->env.hold1_level);
			} else if (env_time <= ta1) {
				x1 = ta;
				y1 = normalize_level(_patch->env.hold1_level);
				x2 = ta1;
				y2 = normalize_level(_patch->env.hold2_level);
			} else if (env_time <= ta12) {
				x1 = ta1;
				y1 = normalize_level(_patch->env.hold2_level);
				x2 = ta12;
				y2 = normalize_level(_patch->env.hold3_level);
			} else if (env_time <= ta123) {
				x1 = ta12;
				y1 = normalize_level(_patch->env.hold3_level);
				x2 = ta123;
				y2 = normalize_level(_patch->env.sustain_level);
			} else {
				x1 = ta123;
				y1 = normalize_level(_patch->env.sustain_level);
				x2 = x1 + 1;
				y2 = y1;
			}
		} else {                    // Note off
			if (env_time <= _patch->env.release) {
				x1 = 0;
				y1 = _actual_sustain_level;
				x2 = _patch->env.release;
				y2 = 0;
			} else {
				x1 = _patch->env.release;
				y1 = 0;
				x2 = x1 + 1;
				y2 = 0;
			}
		}
		// Calculate the envelope level
		env_level = linear_interpolate(x1, y1, x2, y2, env_time);
	} else {                     // Buzzer is on
		env_level = note_on ? 1.0 : 0.0;
	}

	// Increment the envelope sample count
	_env_smp_count++;
}

void Voice::update_ringmod()
{
	// TODO: fix ringmod phasing issue with mono playmod
	update_ringmod_pitch();
	update_ringmod_smp_period();
	update_ringmod_smp_count();
	update_ringmod_waveform_level();
}

void Voice::update_ringmod_pitch()
{
	double fvr = _patch->ringmod.fixed_vs_relative;
	double rp = _patch->ringmod.detune + _final_pitch;
	if (fvr < 1.0) {
		double fp = _patch->ringmod.detune + _patch->ringmod.fixed_pitch;
		_ringmod_pitch = linear_interpolate(0.0, fp, 1.0, rp, fvr);
	} else {
		_ringmod_pitch = rp;
	}
}

void Voice::update_ringmod_smp_period()
{
	_ringmod_whole_smp_period = 2 * _engine->pitch2toneperiod(_ringmod_pitch);
	int nsegs = (RINGMOD_WAVEFORM_SIZE * (_patch->ringmod.loop == RingMod::Loop::PingPong ? 2 : 1));
	_ringmod_smp_period = _ringmod_whole_smp_period	/ nsegs;
}

void Voice::update_ringmod_smp_count()
{
	// Update ringmod sample count and waveform index
	_ringmod_smp_count += _engine->ay.step * DECIMATE_FACTOR;
	double smp_phase = _patch->ringmod.phase * _ringmod_whole_smp_period;
	while (_ringmod_smp_period <= (_ringmod_smp_count + smp_phase)) {
		_ringmod_smp_count -= _ringmod_smp_period;
		update_ringmod_waveform_index();
	}
}

void Voice::update_ringmod_waveform_index()
{
	static unsigned last_index = RINGMOD_WAVEFORM_SIZE - 1;

	// Deal with extremities
	if (_ringmod_waveform_index == 0 and _ringmod_back) {
		// Inverse backward to forward
		_ringmod_back = false;
		return;
	}
	if (_ringmod_waveform_index == last_index) {
		switch(_patch->ringmod.loop) {
		case RingMod::Loop::Off:
			// Remain on the last index indefinitely
			return;
		case RingMod::Loop::Forward:
			// Loop back to beginning
			_ringmod_waveform_index = 0;
			return;
		case RingMod::Loop::PingPong:
			if (not _ringmod_back) {
				// Inverse forward to backward
				_ringmod_back = true;
			} else {
				// Go back
				_ringmod_waveform_index--;
			}
			return;
		default:
			std::cerr << "Case not implemented, there's likely a bug" << std::endl;
			break;
		}
	}

	// Deal with between extremities
	if (_ringmod_back) {
		// Go backward
		_ringmod_waveform_index--;
	} else {
		// Go forward
		_ringmod_waveform_index++;
	}
}

void Voice::update_ringmod_waveform_level()
{
	// Determine ringmod depth, according to fixed depth, velocity and sequencer
	double depth = normalize_level(_patch->ringmod.depth);
	depth *= velocity_to_depth(_patch->control.ringmod_velocity_sensitivity, velocity);
	if (0 <= _seq_index)
		depth *= normalize_level(_patch->seq.states[_seq_index].ringmod_depth);

	// Determine waveform level according to depth and waveform index
	double wfl = normalize_level(_patch->ringmod.waveform[_ringmod_waveform_index]);
	_ringmod_waveform_level = linear_interpolate(0.0, (1.0 - depth), 1.0, 1.0, wfl);
}

void Voice::update_buzzer()
{
	update_buzzer_shape();
	update_buzzer_pitch();
	update_buzzer_period();
	ayumi_set_envelope(&_engine->ay, _buzzer_period);
}

void Voice::update_buzzer_off()
{
	_buzzer_off = note_on ? (not _patch->buzzer.enabled) : true;
}

void Voice::update_buzzer_pitch()
{
	_buzzer_pitch = _ringmod_pitch;
}

void Voice::update_buzzer_period()
{
	double tbp = _buzzer_pitch;
	if (_patch->ringmod.loop == RingMod::Loop::PingPong)
		tbp += 12.0;
	_buzzer_period = _engine->pitch2envperiod(tbp);
}

void Voice::update_buzzer_shape()
{
	if (_patch->buzzer.shape != _engine->buzzershape
	    or _patch->ringmod.loop != _engine->ringmodloop) {
		int ym_shape = 0;
		switch(_patch->buzzer.shape) {
		case Buzzer::Shape::DownSaw:
			switch(_patch->ringmod.loop) {
			case RingMod::Loop::Off:
				ym_shape = 8;		  // TODO
				break;
			case RingMod::Loop::Forward:
				ym_shape = 8;
				break;
			case RingMod::Loop::PingPong:
				ym_shape = 10;
				break;
			}
			break;
		case Buzzer::Shape::UpSaw:
			switch(_patch->ringmod.loop) {
			case RingMod::Loop::Off:
				ym_shape = 12;		  // TODO
				break;
			case RingMod::Loop::Forward:
				ym_shape = 12;
				break;
			case RingMod::Loop::PingPong:
				ym_shape = 14;
				break;
			}
			break;
		default:
			std::cerr << "Case not implemented, there's likely a bug" << std::endl;
			break;
		}
		ayumi_set_envelope_shape(&_engine->ay, ym_shape);
		_engine->ayenvshape = ym_shape;
		_engine->buzzershape = _patch->buzzer.shape;
		_engine->ringmodloop = _patch->ringmod.loop;
	}
}

void Voice::update_seq_level()
{
   _seq_level = 0 <= _seq_index ? normalize_level(_patch->seq.states[_seq_index].level) : 1.0;
}

void Voice::update_final_level()
{
	_final_level = _ringmod_waveform_level * env_level * _seq_level * velocity_level;
}

void Voice::reset_tone()
{
	if (not _patch->tone.reset)
		return;

	struct tone_channel& ch = _engine->ay.channels[ym_channel];
	double tp = ch.tone_period;
	double wtp = 2 * tp;            // Whole tone period
	double counter = std::round(_patch->tone.phase * wtp);
	if (counter < tp) {
		ch.tone = 1;
	} else {
		ch.tone = 0;
		counter -= std::floor(tp);
	}
	ch.tone_counter = counter;
}

float floatrand()
{
	return ((float)rand() / (float)RAND_MAX);
}

void Voice::reset_ringmod()
{
	// Make sure the ring modulation period is correct
	update_ringmod_pitch();
	update_ringmod_smp_period();

	// Update ringmod count to be in sync
	float init_phase = _patch->ringmod.reset ? 0.0f : floatrand();
	_ringmod_smp_count = init_phase * _ringmod_whole_smp_period;
}

void Voice::reset_buzzer()
{
	if (not _patch->ringmod.reset)
		return;

	// Make sure the buzzer period is correct
	update_buzzer_shape();
	update_buzzer_pitch();
	update_buzzer_period();
	_engine->ay.envelope_counter = std::lround(_buzzer_period * _patch->ringmod.phase);
}

void Voice::sync_ringmod()
{
	// Make sure the ring modulation period is correct
	update_ringmod_pitch();
	update_ringmod_smp_period();

	// Update ringmod count to be in sync
	_ringmod_smp_count = 0;
	_ringmod_back = false;
	_ringmod_waveform_index = 0;
}

void Voice::sync_buzzer()
{
	// Make sure the buzzer period is correct
	update_buzzer_shape();
	update_buzzer_pitch();
	update_buzzer_period();
	_engine->ay.envelope_counter = std::lround(_buzzer_period * _patch->ringmod.phase);
}

double Voice::velocity_to_level(double velocity_sensitivity, unsigned char velocity)
{
	return linear_interpolate(1.0, 1.0 - velocity_sensitivity,
	                          127.0, 1.0, (double)velocity);
}

double Voice::velocity_to_depth(double velocity_sensitivity, unsigned char velocity)
{
	return linear_interpolate(1.0, 1.0 - velocity_sensitivity,
	                          127.0 * 127.0, 1.0, (double)((int)velocity * (int)velocity));
}

double Voice::lfo_cycle_remainder(double freq, double time)
{
	// TODO: optimize by maintaining a lfo_time attribute instead.
	double period = 1.0 / freq;
	while (period < time)
		time -= period;
	return time;
}

double Voice::lfo_sine_pitch(double freq, double time)
{
	return sin(2 * M_PI * time * freq);
}

double Voice::lfo_triangle_pitch(double freq, double time)
{
	double period = 1.0 /freq;
	double half_period = period / 2.0;
	double quater_period = half_period / 2.0;
	time += quater_period;       // dephase to start at zero
	time = lfo_cycle_remainder(freq, time);
	if (time < half_period)
		return linear_interpolate(0.0, -1.0, half_period, 1.0, time);
	else
		return linear_interpolate(half_period, 1.0, period, -1.0, time);
}

double Voice::lfo_downsaw_pitch(double freq, double time)
{
	double period = 1.0 /freq;
	double half_period = period / 2.0;
	time += half_period;       // dephase to start at zero
	time = lfo_cycle_remainder(freq, time);
	return linear_interpolate(0.0, 1.0, period, -1.0, time);
}

double Voice::lfo_upsaw_pitch(double freq, double time)
{
	double period = 1.0 /freq;
	double half_period = period / 2.0;
	time += half_period;       // dephase to start at zero
	time = lfo_cycle_remainder(freq, time);
	return linear_interpolate(0.0, -1.0, period, 1.0, time);
}

double Voice::lfo_square_pitch(double freq, double time)
{
	time = lfo_cycle_remainder(freq, time);
	double period = 1.0 /freq;
	double half_period = period / 2.0;
	if (time < half_period)
		return -1.0;
	else
		return 1.0;
}

double Voice::lfo_rand_pitch(double freq, double time)
{
	double period = 1.0 /freq;
	double half_period = period / 2.0;
	uint32_t index = std::floor(time / half_period);
	uint32_t rnd = hash(index);
	static const uint32_t uint32_t_max = std::numeric_limits<uint32_t>::max();
	return linear_interpolate(0, -1.0, uint32_t_max, 1.0, rnd);
}

uint32_t Voice::hash(uint32_t a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

double Voice::normalize_level(int level)
{
	return (double)level / (double)MAX_LEVEL;
}
