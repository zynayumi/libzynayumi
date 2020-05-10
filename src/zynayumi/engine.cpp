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
#include <assert.h>

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/min_element.hpp>

#include "engine.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

// Constructor destructor
Engine::Engine(const Zynayumi& ref)
	: _zynayumi(ref),
	  emulmode(EmulMode::YM2149),
	  playmode(PlayMode::Mono),
	  previous_pitch(-1),
	  last_pitch(-1),
	  lower_note_freq(8.1757989156),
	  sample_rate(44100),        // This is redefined by the host anyway
	  // According to wikipedia
	  // https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910 the
	  // clock rate should be 4MHz for the AY-3-8910 and 8MHz for the
	  // YM2149. However, it seems that ayumi only supports up to 2MHz,
	  // and in http://sovietov.com/app/ayumi/ayumi.html the possible
	  // clock rates are 1.75MHz to 2MHz. Using higher clock rates make
	  // ayumi generate terrible noise, so 2MHz is set and fixed for
	  // now.
	  clock_rate(2000000),
	  mw_depth(0),
	  pw_pitch(0),
	  _max_voices(3)
{
	// TODO: probably unnecessary, as configured when setting the patch
	// or the sample rate.
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
		ayumi_configure(&ay, emulmode == EmulMode::YM2149,
		                clock_rate, sample_rate);
		emulmode = _zynayumi.patch.emulmode;
	}

	// Send off notes in case playmode went from poly to mono
	if (_zynayumi.patch.playmode != playmode) {
		if (playmode == PlayMode::Poly) {
			for (Voice& v : _voices) {
				if (0 < v.ym_channel and v.note_on) {
					v.set_note_off();
				}
			}
		}
		playmode = _zynayumi.patch.playmode;
	}

	for (unsigned long i = 0; i < sample_count; i++) {
		// Update voice states (which modulates the ayumi state)
		for (auto& v : _voices)
			v.update();

		// Remove off voices
		for (auto it = _voices.begin(); it != _voices.end();) {
			if (not it->note_on and it->env_level == 0.0)
				it = _voices.erase(it);
			else ++it;
		}

		// Process ayumi
		ayumi_process(&ay);
		ayumi_remove_dc(&ay);

		// Update outputs
		left_out[i] = std::clamp((float)ay.left * _zynayumi.patch.gain, -1.0f, 1.0f);
		right_out[i] = std::clamp((float)ay.right * _zynayumi.patch.gain, -1.0f, 1.0f);
	}
}

void Engine::noteOn_process(unsigned char channel,
                            unsigned char pitch,
                            unsigned char velocity)
{
	set_last_pitch(pitch);
	pitches.insert(pitch);
	pitch_stack.push_back(pitch);

	switch(_zynayumi.patch.playmode) {
	case PlayMode::Mono:
		if (pitch_stack.size() == 1) {
			// We go from 0 to 1 on note
			free_voice();
			add_voice(pitch, velocity);
		} else {
			// There is already an on note, merely change its pitch
			unsigned char pitch = pitch_stack.back();
			_voices.front().set_note_pitch(pitch);
		}	
		break;
	case PlayMode::MonoUpArp:
	case PlayMode::MonoDownArp:
	case PlayMode::MonoRndArp:
		if (pitches.size() == 1) {
			// We go from 0 to 1 on note
			free_voice();
			add_voice(pitch, velocity);
		};
		break;
	case PlayMode::Poly:
		if ((size_t)_max_voices <= _voices.size())
			free_voice();
		add_voice(pitch, velocity);
		break;
	case PlayMode::Unison:
		if (pitch_stack.size() == 1) {
			// We go from 0 to 1 on note
			free_all_voices();
			add_all_voices(pitch, velocity);
		} else {
			// There is already an on note, merely change its pitch
			unsigned char pitch = pitch_stack.back();
			set_all_voices_with_pitch(pitch);
		}
		break;
	case PlayMode::UnisonUpArp:
	case PlayMode::UnisonDownArp:
	case PlayMode::UnisonRndArp:
		if (pitches.size() == 1) {
			// We go from 0 to 1 on note
			free_all_voices();
			add_all_voices(pitch, velocity);
		};
		break;
	default:
		break;
	}
}

void Engine::noteOff_process(unsigned char channel, unsigned char pitch) {
	auto print_err = [&]() {
		std::cerr << "NoteOff (channel=" << (int)channel << ", pitch="
		          << (int)pitch << ") has no corresponding NoteOn"
		          << std::endl;
	};

	// Remove the corresponding pitch
	auto pit = pitches.find(pitch);
	if (pit != pitches.end()) {
		pitches.erase(pit);
		pitch_stack.erase(boost::find(pitch_stack, pitch));
	} else {
		print_err();
		return;
	}

	// Possibly set the corresponding voice off
	switch(_zynayumi.patch.playmode) {
	case PlayMode::Mono:
	{
		// If the pitch stack is not empty, get the previous pitch and
		// the set the voice with it.
		if (not pitch_stack.empty()) {
			unsigned char prev_pitch = pitch_stack.back();
			set_last_pitch(prev_pitch);
			_voices.front().set_note_pitch(prev_pitch);
		} else {
			set_note_off_with_pitch(pitch);
		}
		break;
	}
	case PlayMode::MonoUpArp:
	case PlayMode::MonoDownArp:
	case PlayMode::MonoRndArp:
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
	case PlayMode::Poly:
	{
		set_note_off_with_pitch(pitch);
		break;
	}
	case PlayMode::Unison:
	{
		// If the pitch stack is not empty, get the previous pitch and
		// the set the voice with it.
		if (not pitch_stack.empty()) {
			unsigned char prev_pitch = pitch_stack.back();
			set_last_pitch(prev_pitch);
			set_all_voices_with_pitch(prev_pitch);
		} else {
			set_note_off_on_all_voices();
		}
		break;
	}
	case PlayMode::UnisonUpArp:
	case PlayMode::UnisonDownArp:
	case PlayMode::UnisonRndArp:
		if (pitches.empty()) {
			set_note_off_on_all_voices();
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
}

void Engine::allNotesOff_process()
{
	set_note_off_all_voices();
}

void Engine::modulation_process(unsigned char channel, unsigned char value)
{
	double ms = _zynayumi.patch.control.modulation_sensitivity;
	mw_depth = Voice::linear_interpolate(0.0, 0.0, 127.0, ms, (double)value);
}

void Engine::pitchWheel_process(unsigned char channel, short value)
{
	static double max_value = std::pow(2.0, 14.0);
	double min_pitch = -(double)_zynayumi.patch.control.pitchwheel;
	double max_pitch = (double)_zynayumi.patch.control.pitchwheel;
	pw_pitch = Voice::linear_interpolate(0.0, min_pitch,
	                                     max_value, max_pitch,
	                                     (double)value);
}

// Print method
void Engine::print(int m) const {
	// TODO
}

double Engine::pitch2period_ym(double pitch) const {
	// We need to divide coef1 by 16.0. I have no explanation for it,
	// but it works this way.
	static const double coef1 = (clock_rate / lower_note_freq) / 16.0;
	static const double coef2 = log(2.0) / 12.0;
	return coef1 * exp(-pitch * coef2);
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

int Engine::select_ym_channel() const
{
	std::set<int> free_channels{0, 1, 2};
	for (const auto& v : _voices)
		free_channels.erase(v.ym_channel);
	int chi = rand() % free_channels.size();
	return *std::next(free_channels.begin(), chi);
}

void Engine::set_last_pitch(unsigned char pitch)
{
	previous_pitch = last_pitch;
	last_pitch = pitch;
}

void Engine::add_voice(unsigned char pitch, unsigned char velocity)
{
	int ym_channel = _zynayumi.patch.playmode == PlayMode::Poly ?
		select_ym_channel() : 0;
	_voices.emplace_back(*this, _zynayumi.patch, ym_channel, pitch, velocity);
}

void Engine::add_all_voices(unsigned char pitch, unsigned char velocity)
{
	_voices.emplace_back(*this, _zynayumi.patch, 0, pitch, velocity);
	_voices.emplace_back(*this, _zynayumi.patch, 1, pitch, velocity);
	_voices.emplace_back(*this, _zynayumi.patch, 2, pitch, velocity);
}

void Engine::free_voice()
{
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
	auto it = boost::min_element(_voices, lt);
	if (it != _voices.end())
		_voices.erase(boost::min_element(_voices, lt));
}

void Engine::free_all_voices() {
	_voices.clear();
}

void Engine::set_all_voices_with_pitch(unsigned char pitch) {
	for (auto& voice : _voices)
		voice.set_note_pitch(pitch);
}

void Engine::set_note_off_with_pitch(unsigned char pitch) {
	for (Voice& v : _voices) {
		if (v.pitch == pitch and v.note_on) {
			v.set_note_off();
			break;
		}
	}
}

void Engine::set_note_off_on_all_voices() {
	for (auto& v : _voices)
		if (v.note_on)
			v.set_note_off();
}

} // ~namespace zynayumi
