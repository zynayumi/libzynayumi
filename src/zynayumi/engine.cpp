/****************************************************************************
    
    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

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

#include <boost/range/algorithm/min_element.hpp>

#include "engine.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

// Constructor destructor
Engine::Engine(const Zynayumi& ref)
	: _zynayumi(ref),
	  previous_pitch(-1),
	  last_pitch(-1),
	  ringmod_smp_count{0.0, 0.0, 0.0},
	  ringmod_waveform_index{0, 0, 0},
	  lower_note_freq(8.1757989156),
	  // In principle it should be 8.1757989156 as in
	  // http://subsynth.sourceforge.net/midinote2freq.html. But for
	  // some reason it's out of tune so we found this value by
	  // bisective search using xsynth.
	  lower_note_freq_ym(2.88310682560),
	  sample_rate(44100),
	  clock_rate(2000000),
	  _max_voices(3) {
	ayumi_configure(&ay, 1, clock_rate, sample_rate);
}

void Engine::audio_process(float* left_out, float* right_out,
                           unsigned long sample_count) {
	for (unsigned long i = 0; i < sample_count; i++) {
		// Update voice states (which modulates the ayumi state)
		for (Pitch2Voice::value_type& v : _voices)
			v.second.update();

		// Remove off voices
		for (Pitch2Voice::iterator it = _voices.begin(); it != _voices.end();) {
			if (not it->second.note_on and it->second.env_level == 0.0)
				it = _voices.erase(it);
			else ++it;
		}

		if (not _voices.empty()) {
			// Update ayumi state
			ayumi_process(&ay);
			ayumi_remove_dc(&ay);

			// Update outputs
			left_out[i] = (float) ay.left;
			right_out[i] = (float) ay.right;
		}
		else {
			left_out[i] = 0.0f;
			right_out[i] = 0.0f;
		}
	}
}

void Engine::noteOn_process(unsigned char channel,
                            unsigned char pitch,
                            unsigned char velocity) {
	previous_pitch = last_pitch;
	last_pitch = pitch;

	pitches.insert(pitch);

	// If no voice
	if (_voices.empty()) {
		add_voice(pitch, velocity);
		return;
	}

	// If we're here there is at least a voice on
	switch(_zynayumi.patch.playmode) {
	case PlayMode::Mono:
		free_voice();
		add_voice(pitch, velocity);
		break;
	case PlayMode::Poly:
		if ((size_t)_max_voices <= _voices.size())
			free_voice();
		add_voice(pitch, velocity);
		break;
	case PlayMode::UpArp:
	case PlayMode::DownArp:
	case PlayMode::RndArp:
		if (pitches.size() == 1) {
			free_voice();
			add_voice(pitch, velocity);
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
	} else {
		print_err();
		return;
	}

	// Possibly set the corresponding voice off
	switch(_zynayumi.patch.playmode) {
	case PlayMode::Mono:
	case PlayMode::Poly:
	{
		// Set the corresponding voice off, if it hasn't been removed
		// by a new voice.
		if (_voices.find(pitch) != _voices.end()) {
			auto vitr = _voices.equal_range(pitch);
			for (auto vit = vitr.first; vit != vitr.second; ++vit) {
				if (vit->second.note_on) {
					vit->second.set_note_off();
					break;
				}
			}
		}
		break;
	}
	case PlayMode::UpArp:
	case PlayMode::DownArp:
	case PlayMode::RndArp:
		if (pitches.empty()) {
			assert(_voices.size() == 1);
			_voices.begin()->second.set_note_off();
		} else if (pitches.size() == 1) {
			_voices.begin()->second.set_note_pitch(*pitches.begin());
		}
		break;
	default:
		break;
	}
}

void Engine::allNotesOff_process() {
	for (auto& voice : _voices)
		voice.second.set_note_off();
}

// Print method
void Engine::print(int m) const {
	// TODO
}

double Engine::pitch2period(double pitch) const {
	static double coef1 = sample_rate / lower_note_freq;
	static double coef2 = log(2.0) / 12.0;
	return coef1 * exp(-pitch * coef2);
}

double Engine::pitch2period_ym(double pitch) const {
	static double coef1 = sample_rate / lower_note_freq_ym;
	static double coef2 = log(2.0) / 12.0;
	return coef1 * exp(-pitch * coef2);
}

double Engine::smp2sec(unsigned long long smp_count) const {
	return (double)smp_count / (double)sample_rate;
}

int Engine::select_ym_channel() const {
	std::set<int> free_channels{0, 1, 2};
	for (const Pitch2Voice::value_type& pv : _voices)
		free_channels.erase(pv.second.channel);
	int chi = rand() % free_channels.size();
	return *std::next(free_channels.begin(), chi);
}

void Engine::add_voice(unsigned char pitch, unsigned char velocity) {
	int channel = _zynayumi.patch.playmode == PlayMode::Poly ?
		select_ym_channel() : 0;
	Voice voice(*this, _zynayumi.patch, channel, pitch, velocity);
	_voices.emplace(pitch, voice);
}

void Engine::free_voice() {
	// Select voice with the lowest velocity
	Pitch2Voice::const_iterator it =
		boost::min_element(_voices, [](const Pitch2Voice::value_type& v1,
		                               const Pitch2Voice::value_type& v2)
		                   { return v1.second.env_level < v2.second.env_level; });
	_voices.erase(it);
}

} // ~namespace zynayumi
