/****************************************************************************
    
    Presets for Zynayumi

    presets.cpp

    Copyleft (c) 2017 Nil Geisweiller
 
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

#include "presets.hpp"

namespace zynayumi {

Presets::Presets() {
	{
		// Power bass
		Patch patch;
		patch.name = "Power bass";
		patch.ampenv.time3 = 5.0;
		patch.ampenv.sustain_level = 0.5;
		patch.ampenv.release = 0.05;
		patch.pitchenv.attack_pitch = 24.0;
		patch.pitchenv.time = 0.1;
		patch.ringmod.waveform[1] = 0.7;
		patch.ringmod.waveform[5] = 0.5;
		patch.ringmod.waveform[6] = 0.3;
		patch.ringmod.waveform[7] = 0.0;
		patch.ringmod.detune = 0.05;
		patch.lfo.freq = 0.4;
		patch.lfo.delay = 10.0;
		patch.lfo.depth = 0.2;
		patches.push_back(patch);
	}
}

} // ~namespace zynayumi
