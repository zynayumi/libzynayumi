/****************************************************************************

    Zynayumi Synth based on ayumi, a highly precise emulation of the
    YM2149 and AY-3-8910

    engine.cpp

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
#include <sstream>
#include <assert.h>

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/min_element.hpp>
#include <boost/range/algorithm/count.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include "engine.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

// Constructor destructor
Engine::Engine(const Zynayumi& ref)
	: _zynayumi(ref),
	  emulmode(EmulMode::YM2149),
	  cantusmode(CantusMode::Mono),
	  playmode(PlayMode::Legato),
	  buzzershape(Buzzer::Shape::Count),
	  ringmodloop(RingMod::Loop::Count),
	  ayenvshape(0),
	  previous_pitch(-1),
	  last_pitch(-1),
	  lower_note_freq(8.1757989156),
	  // According to wikipedia
	  // https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910 the
	  // clock rate should be 4MHz for the AY-3-8910 and 8MHz for the
	  // YM2149.  However in http://users.rcn.com/carlott/ay3-8910.pdf
	  // they consistently give 2MHz as example of clock rate.  Also,
	  // from Arkos Tracker 2 http://www.julien-nevo.com/arkostracker/
	  // the clock is set to 1MHz for the Amdstrad CPC and 2MHz for the
	  // Atari ST, so we're going with that.
	  clock_rate(YM2149_CLOCK_RATE),
	  sample_rate(44100),        // Nornally redefined by the host
	  bpm(120),                  // Normally redefined by the host
	  pw_pitch(0),
	  mw_depth(0),
	  portamento_time(0),
	  volume_gain(vol2gain(100)),
	  pan(0.5),
	  expression_gain(vol2gain(127)),
	  sustain_pedal(false),
	  oversampling(2)
{
	_voices.emplace_back(*this, _zynayumi.patch, 0);
	_voices.emplace_back(*this, _zynayumi.patch, 1);
	_voices.emplace_back(*this, _zynayumi.patch, 2);
	ayumi_configure(&ay, 1, clock_rate, sample_rate);
}

void Engine::set_sample_rate(int sr)
{
	sample_rate = sr;
	ayumi_configure(&ay, emulmode == EmulMode::YM2149,
	                clock_rate, sample_rate);
}

void Engine::set_bpm(double b)
{
	bpm = b;
}

void Engine::audio_process(float* left_out, float* right_out,
                           unsigned long sample_count)
{
	// Switch to the correct emulation mode (YM2149 or YM8910)
	if (_zynayumi.patch.emulmode != emulmode) {
		bool is_ym2149 = _zynayumi.patch.emulmode == EmulMode::YM2149;
		clock_rate = is_ym2149 ? YM2149_CLOCK_RATE : AY8910_CLOCK_RATE;
		ayumi_configure(&ay, is_ym2149, clock_rate, sample_rate);
		ayumi_set_envelope_shape(&ay, ayenvshape);
		emulmode = _zynayumi.patch.emulmode;
	}

	// Send off notes in case cantusmode went from poly to mono or unison
	if (_zynayumi.patch.cantusmode != cantusmode) {
		if (cantusmode == CantusMode::Poly) {
			bool skip_first_enabled = true;
			for (Voice& v : _voices) {
				// Skip the note on the first enabled ym channel since it
				// should still be on in mono cantusmode.
				if (v.enabled and skip_first_enabled) {
					skip_first_enabled = false;
					break;
				}
				if (v.enabled and v.note_on) {
					v.set_note_off();
				}
			}
		}
		cantusmode = _zynayumi.patch.cantusmode;
	}

	for (unsigned long i = 0; i < sample_count; i++) {
		// Update voice states (which modulates the ayumi state)
		for (Voice& v : _voices)
			v.update();

		// Process ayumi
		ayumi_process(&ay);
		ayumi_remove_dc(&ay);

		// Update outputs
		left_out[i] = (float)ay.left * (1.0f - pan) *
			_zynayumi.patch.mixer.gain * volume_gain * expression_gain;
		right_out[i] = (float)ay.right * pan *
			_zynayumi.patch.mixer.gain * volume_gain * expression_gain;
	}
}

void Engine::note_on_process(unsigned char /* channel */,
                             unsigned char pitch,
                             unsigned char velocity)
{
	set_last_pitch(pitch);
	insert_pitch(pitch, velocity);
	if (sustain_pedal)
		erase_sustain_pitch(pitch);

	switch(_zynayumi.patch.cantusmode) {
	case CantusMode::Mono:
		switch(_zynayumi.patch.playmode) {
		case PlayMode::Legato:
		case PlayMode::Retrig:
			if (pitch_stack.size() == 1) {
				// We go from 0 to 1 on note
				add_voice(pitch, velocity);
			} else {
				// There is already an on note, merely change its pitch
				unsigned char pitch = pitch_stack.back();
				int first_enabled_ym_channel = select_ym_channel(false);
				if (0 <= first_enabled_ym_channel) {
					_voices[first_enabled_ym_channel].set_note_pitch(pitch);
					if (_zynayumi.patch.playmode == PlayMode::Retrig) {
						_voices[first_enabled_ym_channel].set_velocity(velocity);
						_voices[first_enabled_ym_channel].retrig();
					}
				}
			}
			break;
		case PlayMode::UpArp:
		case PlayMode::DownArp:
		case PlayMode::RandArp:
			if (pitches.size() == 1) {
				// We go from 0 to 1 on note
				add_voice(pitch, velocity);
			};
			break;
		default:
			break;
		}
		break;
	case CantusMode::Unison:
		switch(_zynayumi.patch.playmode) {
		case PlayMode::Legato:
		case PlayMode::Retrig:
			if (pitch_stack.size() == 1) {
				// We go from 0 to 1 on note
				add_all_voices(pitch, velocity);
			} else {
				// There is already an on note, merely change its pitch
				unsigned char pitch = pitch_stack.back();
				set_all_voices_pitch(pitch);
				if (_zynayumi.patch.playmode == PlayMode::Retrig) {
					set_all_voices_velocity(velocity);
					retrig_all_voices();
				}
			}
			break;
		case PlayMode::UpArp:
		case PlayMode::DownArp:
		case PlayMode::RandArp:
			if (pitches.size() == 1) {
				// We go from 0 to 1 on note
				add_all_voices(pitch, velocity);
			};
			break;
		default:
			break;
		}
		break;
	case CantusMode::Poly:
		add_voice(pitch, velocity);
		break;
	default:
		break;
	}
}

void Engine::note_off_process(unsigned char /* channel */, unsigned char pitch)
{
	// If sustain pedal is on then ignore the off, but save it for when
	// the pedal will go iff
	if (sustain_pedal) {
		insert_sustain_pitch(pitch);
		return;
	}

	// Otherwise erase the corresponding pitch
	erase_pitch(pitch);

	// Possibly set the corresponding voice off
	switch(_zynayumi.patch.cantusmode) {
	case CantusMode::Mono:
		switch(_zynayumi.patch.playmode) {
		case PlayMode::Legato:
		case PlayMode::Retrig:
		{
			// If the pitch stack is not empty, get the previous pitch and
			// the set the voice with it.
			if (not pitch_stack.empty()) {
				unsigned char prev_pitch = pitch_stack.back();
				unsigned char prev_vel = velocity_stack.back();
				set_last_pitch(prev_pitch);
				int first_enabled_ym_channel = select_ym_channel(false);
				if (0 <= first_enabled_ym_channel) {
					_voices[first_enabled_ym_channel].set_note_pitch(prev_pitch);
					if (_zynayumi.patch.playmode == PlayMode::Retrig) {
						_voices[first_enabled_ym_channel].set_velocity(prev_vel);
						_voices[first_enabled_ym_channel].retrig();
					}
				}
			} else {
				set_note_off_with_pitch(pitch);
			}
			break;
		}
		case PlayMode::UpArp:
		case PlayMode::DownArp:
		case PlayMode::RandArp:
			if (pitches.empty()) {
				set_note_off_with_pitch(pitch);
			} else if (pitches.size() == 1) {
				unsigned char last_pitch = *pitches.begin();
				for (Voice& v : _voices) {
					if (v.note_on) {
						v.set_note_pitch(last_pitch);
						break;
					}
				}
			}
			break;
		default:
			break;
		}
		break;
	case CantusMode::Unison:
		switch(_zynayumi.patch.playmode) {
		case PlayMode::Legato:
		case PlayMode::Retrig:
		{
			// If the pitch stack is not empty, get the previous pitch and
			// the set the voice with it.
			if (not pitch_stack.empty()) {
				unsigned char prev_pitch = pitch_stack.back();
				unsigned char prev_vel = velocity_stack.back();
				set_last_pitch(prev_pitch);
				set_all_voices_pitch(prev_pitch);
				if (_zynayumi.patch.playmode == PlayMode::Retrig) {
					set_all_voices_velocity(prev_vel);
					retrig_all_voices();
				}
			} else {
				set_note_off_all_voices();
			}
			break;
		}
		case PlayMode::UpArp:
		case PlayMode::DownArp:
		case PlayMode::RandArp:
			if (pitches.empty()) {
				set_note_off_all_voices();
			} else if (pitches.size() == 1) {
				unsigned char last_pitch = *pitches.begin();
				for (Voice& v : _voices) {
					if (v.note_on) {
						v.set_note_pitch(last_pitch);
						break;
					}
				}
			}
			break;
		default:
			break;
		}
		break;
	case CantusMode::Poly:
	{
		set_note_off_with_pitch(pitch);
		break;
	}
	default:
		break;
	}
}

void Engine::all_notes_off_process()
{
	pitches.clear();
	pitch_stack.clear();
	velocity_stack.clear();
	sustain_pitches.clear();
	set_note_off_all_voices();
}

void Engine::pitch_wheel_process(unsigned char /* channel */, short value)
{
	static double max_value = std::pow(2.0, 14.0);
	double min_pitch = -(double)_zynayumi.patch.control.pitchwheel;
	double max_pitch = (double)_zynayumi.patch.control.pitchwheel;
	pw_pitch = Voice::linear_interpolate(0.0, min_pitch,
	                                     max_value, max_pitch,
	                                     (double)value);
}

void Engine::modulation_process(unsigned char /* channel */, unsigned char value)
{
	double ms = _zynayumi.patch.control.modulation_sensitivity;
	mw_depth = Voice::linear_interpolate(0.0, 0.0, 127.0, ms, (double)value);
}

void Engine::portamento_process(unsigned char /* channel */, unsigned char value)
{
	// Cubic mapping
	portamento_time = 2.0f *
		((float)value*(float)value*(float)value) / (127.0f*127.0f*127.0f);
}

void Engine::volume_process(unsigned char /* channel */, unsigned char value)
{
	volume_gain = vol2gain(value);
}

void Engine::pan_process(unsigned char /* channel */, unsigned char value)
{
	pan = value == 127 ? 1.0 : (value / 128.0f);
}

void Engine::expression_process(unsigned char /* channel */, unsigned char value)
{
	expression_gain = vol2gain(value);
}

void Engine::sustain_pedal_process(unsigned char channel, unsigned char value)
{
	sustain_pedal = 64 <= value;
	if (not sustain_pedal) {
		for (auto it = sustain_pitches.begin(); it != sustain_pitches.end();) {
			unsigned char pitch = *it;
			it = erase_sustain_pitch(pitch);
			note_off_process(channel, pitch);
		}
	}
}

void Engine::enable_ym_channel(unsigned char ym_channel)
{
	_voices[ym_channel].enable();
}

void Engine::disable_ym_channel(unsigned char ym_channel)
{
	_voices[ym_channel].disable();
}

std::string Engine::to_string(const std::string& indent) const
{
	std::string di = indent + indent;
	std::stringstream ss;
	ss << indent << "pitches:";
	for (unsigned char p : pitches)
		ss << " " << (int)p;
	ss << std::endl;
	ss << indent << "pitch_stack:";
	for (unsigned char p : pitch_stack)
		ss << " " << (int)p;
	ss << std::endl;
	ss << indent << "velocity_stack:";
	for (unsigned char v : velocity_stack)
		ss << " " << (int)v;
	ss << std::endl;
	ss << indent << "sustain pitches:";
	for (unsigned char p : sustain_pitches)
		ss << " " << (int)p;
	ss << std::endl;
	ss << indent << "previous_pitch = " << previous_pitch << std::endl;
	ss << indent << "last_pitch = " << last_pitch;
	return ss.str();
}

double Engine::pitch2toneperiod(double pitch) const
{
	// We need to divide coef1 by 16.0, as explained in
	// http://ym2149.com/ym2149.pdf page 5.
	const double coef1 = (clock_rate / lower_note_freq) / 16.0;
	static const double coef2 = log(2.0) / 12.0;
	double pym = coef1 * exp(-pitch * coef2);
	if (_zynayumi.patch.tone.legacy_tuning)
		pym = std::round(pym);
	return pym;
}

int Engine::pitch2envperiod(double pitch) const
{
	// We need to divide coef1 by 256.0, as explained in
	// http://ym2149.com/ym2149.pdf page 7.
	const double coef1 = (clock_rate / lower_note_freq) / 256.0;
	static const double coef2 = log(2.0) / 12.0;
	double pym = coef1 * exp(-pitch * coef2);
	return std::lround(pym);
}

double Engine::freq2pitch(double freq) const
{
	// Based on formula
	//
	// freq = lowfreq * exp(pitch * (log(2.0) / 12.0))
	// ...
	// pitch = 12.0 * log(freq / lowfreq) / log(2.0)
	static const double coef = 12.0 / log(2.0);
	return coef * log(freq / lower_note_freq);
}

double Engine::smp2sec(unsigned long long smp_count) const
{
	return (double)smp_count / (double)sample_rate;
}

float Engine::vol2gain(short value)
{
	return ((float)value*(float)value) / (127.0f*127.0f);
}

int Engine::select_ym_channel(bool poly) const
{
	// NEXT: implement control.midi_ch
	std::set<unsigned char> enabled_ym_channels = get_enabled_ym_channels();

	// No available ym channel, selection failed.
	if (enabled_ym_channels.empty())
		return -1;

	// Not polyphonic, return the first enabled one
	unsigned char first_enabled_ym_channel = *enabled_ym_channels.begin();
	if (not poly)
		return first_enabled_ym_channel;

	// Determine silent ym channels for poly selection
	std::set<unsigned char> silent_channels;
	for (unsigned char ymch : enabled_ym_channels)
		if (_voices[ymch].is_silent())
			silent_channels.insert(ymch);

	// If no silent channel are available then select the least
	// significant one
	if (silent_channels.empty()) {
		auto lt = [](const Voice& v1, const Voice& v2) {
			if (v1.note_on) {
				if (v2.note_on)
					return v1.on_time > v2.on_time;
				return false;
			}
			else {
				if (v2.note_on)
					return true;
				return v1.env_level < v2.env_level;
			}
		};
		unsigned char least_significant_channel = first_enabled_ym_channel;
		for (unsigned char i : enabled_ym_channels)
			if (lt(_voices[i], _voices[least_significant_channel]))
				least_significant_channel = i;
		return least_significant_channel;
	} else {
		// Otherwise select randomly among the silent ones
		int rchi = rand() % silent_channels.size(); // NEXT: replace rand() by Voice::hash
		return *std::next(silent_channels.begin(), rchi);
	}
}

std::set<unsigned char> Engine::get_enabled_ym_channels() const
{
	std::set<unsigned char> enabled_ym_channels;
	for (const Voice& v : _voices)
		if (v.enabled)
			enabled_ym_channels.insert((unsigned char)v.ym_channel);
	return enabled_ym_channels;
}

void Engine::set_last_pitch(unsigned char pitch)
{
	previous_pitch = last_pitch;
	last_pitch = pitch;
}

void Engine::add_voice(unsigned char pitch, unsigned char velocity)
{
	int ym_channel = select_ym_channel(_zynayumi.patch.cantusmode == CantusMode::Poly);
	if (0 <= ym_channel)
		_voices[ym_channel].set_note_on(pitch, velocity);
}

void Engine::add_all_voices(unsigned char pitch, unsigned char velocity)
{
	for (size_t i = 0; i < 3; i++)
		_voices[i].set_note_on(pitch, velocity);
}

void Engine::set_all_voices_pitch(unsigned char pitch)
{
	for (Voice& voice : _voices) {
		voice.set_note_pitch(pitch);
	}
}

void Engine::set_all_voices_velocity(unsigned char velocity)
{
	for (Voice& voice : _voices) {
		voice.set_velocity(velocity);
	}
}

void Engine::retrig_all_voices()
{
	for (Voice& voice : _voices)
		voice.retrig();
}

void Engine::set_note_off_with_pitch(unsigned char pitch)
{
	// Set all voices off with this pitch, not just one, as it seems to
	// be the standard to deal with overlapping notes of same pitch, if
	// one of them goes off.
	for (Voice& v : _voices) {
		if (v.pitch == pitch and v.note_on) {
			v.set_note_off();
		}
	}
}

void Engine::set_note_off_all_voices()
{
	for (Voice& v : _voices)
		if (v.note_on)
			v.set_note_off();
}

void Engine::insert_pitch(unsigned char pitch, unsigned char velocity)
{
	pitches.insert(pitch);
	pitch_stack.push_back(pitch);
	velocity_stack.push_back(velocity);
}

void Engine::erase_pitch(unsigned char pitch)
{
	auto range = pitches.equal_range(pitch);
	pitches.erase(range.first, range.second);
	boost::remove_erase(pitch_stack, pitch);
	if (not velocity_stack.empty())
		velocity_stack.pop_back();
}

void Engine::insert_sustain_pitch(unsigned char pitch)
{
	sustain_pitches.insert(pitch);
}

std::multiset<unsigned char>::iterator
Engine::erase_sustain_pitch(unsigned char pitch)
{
	auto range = sustain_pitches.equal_range(pitch);
	return sustain_pitches.erase(range.first, range.second);
}

} // ~namespace zynayumi
