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
#include "engine.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

// Constructor destructor
Engine::Engine(const Zynayumi& ref)	: zynayumi(ref),
	  _pitch(-1),
	  // In principle it should be 8.1757989156 as in
	  // http://subsynth.sourceforge.net/midinote2freq.html. But for
	  // some reason it's out of tune so we found this value by
	  // bisective search using xsynth.
	  LOWER_NOTE_FREQ(2.88310683),
	  SAMPLE_RATE(44100),
	  CLOCK_RATE(2000000) {
	ayumi_configure(&_ay, 1, CLOCK_RATE, SAMPLE_RATE);
	ayumi_set_pan(&_ay, 0, 0.5, 0);
	ayumi_set_mixer(&_ay, 0, 0, 1, 0);

	// Tweak patch for testing
}

void Engine::audio_process(float* left_out, float* right_out,
                           unsigned long sample_count) {
	for (unsigned long i = 0; i < sample_count; i++) {
		ayumi_process(&_ay);
		ayumi_remove_dc(&_ay);
		left_out[i] = (float) _ay.left;
		right_out[i] = (float) _ay.right;
	}
}

void Engine::noteOn_process(unsigned char channel,
                            unsigned char pitch,
                            unsigned char velocity) {
	_pitch = (char)pitch;
	ayumi_set_tone(&_ay, 0, (int)pitch2period(_pitch));
	ayumi_set_volume(&_ay, 0, velocity / 8);
}

void Engine::noteOff_process(unsigned char channel, unsigned char pitch) {
	if ((char)pitch == _pitch) {
		ayumi_set_volume(&_ay, 0, 0);
		_pitch = -1;
	}
}

// Print method
void Engine::print(int m) const {
	// TODO
}

float Engine::pitch2period(float pitch)  {
	static float coef1 = SAMPLE_RATE / LOWER_NOTE_FREQ;
	static float coef2 = log(2.0) / 12.0;
	return coef1 * exp(-pitch * coef2);
}

} // ~namespace zynauimi
