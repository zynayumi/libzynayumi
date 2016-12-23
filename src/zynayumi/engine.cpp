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
Engine::Engine(const Zynayumi& ref) : zynayumi(ref) {
	ayumi_configure(&_ayumi, true, CLOCK_RATE, SAMPLE_RATE);
}

void Engine::audio_process(float* left_out, float* right_out,
                           unsigned long sample_count) {
	for (unsigned long i = 0; i < sample_count; i++) {
		ayumi_process(&_ayumi);
		left_out[i] = (float) _ayumi.left;
		right_out[i] = (float) _ayumi.right;
	}
}

void Engine::noteOn_process(unsigned char channel,
                            unsigned char pitch,
                            unsigned char velocity) {
	ayumi_set_tone(&_ayumi, 0, (int)pitch2period(pitch));
	ayumi_set_volume(&_ayumi, 0, velocity / 8);
}

void Engine::noteOff_process(unsigned char channel, unsigned char pitch) {
	ayumi_set_volume(&_ayumi, 0, 0);
}

// Print method
void Engine::print(int m) const {
	// TODO
}

float Engine::pitch2period(float pitch)  {
	static float coef = SAMPLE_RATE / LOWER_NOTE_FREQ;
	return coef * exp(-pitch * LOG2 / 12.0);
}

} // ~namespace zynauimi
