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

#include <boost/range/algorithm/min_element.hpp>

#include "engine.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

// Constructor destructor
Engine::Engine(const Zynayumi& ref)
	: _zynayumi(ref),
	  // In principle it should be 8.1757989156 as in
	  // http://subsynth.sourceforge.net/midinote2freq.html. But for
	  // some reason it's out of tune so we found this value by
	  // bisective search using xsynth.
	  lower_note_freq(2.88310683),
	  sample_rate(44100),
	  clock_rate(2000000),
	  _max_voices(1) {
	ayumi_configure(&ay, 1, clock_rate, sample_rate);
	ayumi_set_pan(&ay, 0, 0.5, 0);
	ayumi_set_mixer(&ay, 0, 0, 1, 0);
}

void Engine::audio_process(float* left_out, float* right_out,
                           unsigned long sample_count) {
	for (unsigned long i = 0; i < sample_count; i++) {
		// Update voice states (which modulates the ayumi state)
		for (Pitch2Voice::value_type& v : _voices)
			v.second.update();

		// Update ayumi state
		ayumi_process(&ay);
		ayumi_remove_dc(&ay);

		// Update outputs
		left_out[i] = (float) ay.left;
		right_out[i] = (float) ay.right;
	}
}

void Engine::noteOn_process(unsigned char channel,
                            unsigned char pitch,
                            unsigned char velocity) {
	pitches.insert(pitch);

	// If all voices are used then free one
	if ((size_t)_max_voices <= _voices.size()) {
		// Select voice with the lowest velocity
		Pitch2Voice::const_iterator it =
			boost::min_element(_voices, [](const Pitch2Voice::value_type& v1,
			                               const Pitch2Voice::value_type& v2)
			                   { return v1.second.env_level < v2.second.env_level; });
		_voices.erase(it);
	}

	_voices.emplace(pitch, Voice(*this, _zynayumi.patch, pitch, velocity));
}

void Engine::noteOff_process(unsigned char channel, unsigned char pitch) {
	auto print_err = [&]() {
		std::cerr << "NoteOff (channel=" << channel
		          << ", pitch=" << pitch << ") has no corresponding NoteOn"
		          << std::endl;
	};

	auto pit = pitches.find(pitch);
	if (pit != pitches.end()) {
		pitches.erase(pit);
	} else {
		print_err();
		return;
	}

	auto vit = _voices.find(pitch);
	if (vit != _voices.end())
		vit->second.set_note_off();
	else {
		print_err();
		return;
	}
}

// Print method
void Engine::print(int m) const {
	// TODO
}

float Engine::pitch2period(float pitch)  {
	static float coef1 = sample_rate / lower_note_freq;
	static float coef2 = log(2.0) / 12.0;
	return coef1 * exp(-pitch * coef2);
}

} // ~namespace zynayumi
