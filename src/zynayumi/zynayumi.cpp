/****************************************************************************
    
    Zynayumi Synth based on ayumi, a highly precise emulation of the YM2149

    zynayumi.cpp

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

#include <cstdio>
#include <iostream>
#include <sstream>

#include "zynayumi.hpp"

using namespace zynayumi;

Zynayumi::Zynayumi() : engine(*this) {}

Zynayumi::~Zynayumi() {}

void Zynayumi::set_sample_rate(int sr)
{
	engine.set_sample_rate(sr);
}

unsigned Zynayumi::get_sample_rate() const
{
	return engine.sample_rate;
}

void Zynayumi::set_bpm(double bpm)
{
	engine.set_bpm(bpm);
}

double Zynayumi::get_bpm() const
{
	return engine.bpm;
}

void Zynayumi::audio_process(float* left_out, float* right_out,
                             unsigned long sample_count)
{
	engine.audio_process(left_out, right_out, sample_count);
}

void Zynayumi::note_on_process(unsigned char channel,
                               unsigned char pitch,
                               unsigned char velocity)
{
	engine.note_on_process(channel, pitch, velocity);
}

void Zynayumi::note_off_process(unsigned char channel, unsigned char pitch)
{
	engine.note_off_process(channel, pitch);
}

void Zynayumi::all_notes_off_process()
{
	engine.all_notes_off_process();
}

void Zynayumi::pitch_wheel_process(unsigned char channel, short value)
{
	engine.pitch_wheel_process(channel, value);
}

void Zynayumi::modulation_process(unsigned char channel, unsigned char value)
{
	engine.modulation_process(channel, value);
}

void Zynayumi::portamento_process(unsigned char channel, unsigned char value)
{
	engine.portamento_process(channel, value);
}

void Zynayumi::volume_process(unsigned char channel, unsigned char value)
{
	engine.volume_process(channel, value);
}

void Zynayumi::pan_process(unsigned char channel, unsigned char value)
{
	engine.pan_process(channel, value);
}

void Zynayumi::expression_process(unsigned char channel, unsigned char value)
{
	engine.expression_process(channel, value);
}

void Zynayumi::sustain_pedal_process(unsigned char channel, unsigned char value)
{
	engine.sustain_pedal_process(channel, value);
}

//print method
std::string Zynayumi::to_string(const std::string& indent) const
{
	std::stringstream ss;
	ss << indent << "engine:" << std::endl;
	ss << engine.to_string(indent + "  ");
	return ss.str();
}
