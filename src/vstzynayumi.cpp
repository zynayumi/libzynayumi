/****************************************************************************
    
    VST wrapper for Zynayumi

    vstzynayumi.cpp

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

#include "pluginterfaces/vst2.x/aeffectx.h"

#include "zynayumi/patch.hpp"
#include "zynayumi/parameters.hpp"

#include "vstzynayumi.hpp"

using namespace zynayumi;

// Main function
extern "C"
{
#define VST_EXPORT __attribute__ ((visibility ("default")))

	extern VST_EXPORT AEffect * VSTPluginMain(audioMasterCallback audioMaster);

	AEffect* main_plugin(audioMasterCallback audioMaster) asm ("main");
#define main main_plugin

	VST_EXPORT AEffect *main(audioMasterCallback audioMaster)
	{
		return VSTPluginMain(audioMaster);
	}
}

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	return new VSTZynayumi(audioMaster);
}

// Plugin implementation

VSTZynayumi::VSTZynayumi(audioMasterCallback audioMaster)
	: AudioEffectX(audioMaster, 1, PARAMETERS_COUNT)
	, events(nullptr)
	, _parameters(zynayumi)
{
	// Plugin id
	setUniqueID(CCONST('Z', 'y', 'N', 'a'));

	// stereo output
	setNumInputs(0);	
	setNumOutputs(2);	
	isSynth();
	canProcessReplacing();
}

VSTZynayumi::~VSTZynayumi()
{
}

void VSTZynayumi::processEvent(VstEvent* e) {
	if (e->type == kVstMidiType) {
		VstMidiEvent* midi_e = (VstMidiEvent*) e;
		char* midiData = midi_e->midiData;
		midi(midiData[0], midiData[1], midiData[2]);
	}
	else if (e->type == kVstSysExType) {
		VstMidiSysexEvent* sysex_e = (VstMidiSysexEvent*) e;
		zynayumi.sysex_process(sysex_e->dumpBytes,
		                       (unsigned char*)sysex_e->sysexDump);
	}
	else std::cerr << "Vst event of type " << e->type
	               << " is not implemented" << std::endl;
}

VstInt32 VSTZynayumi::processEvents(VstEvents* ev) {
	for (VstInt32 i = 0; i < ev->numEvents; i++) {
		processEvent(ev->events[i]);
	}
	return 1;
}

void VSTZynayumi::midi(unsigned char status,
                       unsigned char byte1, unsigned char byte2)
{
	// Ignore midi channel
	status &= 0xf0;
	switch (status) {
	case MSC_NOTE_ON:
	case MSC_NOTE_OFF: {
		unsigned char pitch = byte1, velocity = byte2;
		if (status == MSC_NOTE_ON and velocity > 0)
			zynayumi.noteOn_process(0, pitch, velocity);
		else if (status == MSC_NOTE_OFF or
		         (status == MSC_NOTE_ON and velocity == 0))
			zynayumi.noteOff_process(0, pitch);
		break;
	}
	case MSC_PITCH_WHEEL:
		zynayumi.pitchWheel_process(0, ((short)byte2 << 7) + (short)byte1);
		break;
	case MSC_CONTROL: {
		unsigned char cc = byte1;
		unsigned char value = byte2;
		switch (cc) {
		case CTL_MODWHEEL:
			zynayumi.modulation_process(0, value);
			break;
		case CTL_PORTAMENTO_TIME:
			zynayumi.portamento_process(0, value);
			break;
		case CTL_MAIN_VOLUME:
			zynayumi.volume_process(0, value);
			break;
		case CTL_PAN:
			zynayumi.pan_process(0, value);
			break;
		case CTL_EXPRESSION:
			zynayumi.expression_process(0, value);
			break;
		case CTL_DAMPER_PEDAL:
			zynayumi.sustain_pedal_process(0, value);
			break;
		case CTL_ALL_NOTES_OFF:
			zynayumi.allNotesOff_process();
			break;
		default:
			std::cerr << "Control change " << (int)cc << " unsupported" << std::endl;
		}
		break;
	}
	default:
		std::cerr << "Midi event (status=" << (int)status
		          << ", byte1=" << (int)byte1
		          << ", byte2=" << (int)byte2
		          << ") not implemented" << std::endl;
	}
}

void VSTZynayumi::process(float **inputs, float **outputs,
                          VstInt32 sampleFrames)
{
	int i, cue, block;
	
	// Outputs buffers
	float* p1 = outputs[0];
	float* p2 = outputs[1];

	// Process audio on midi events
	if (events)
	{
		cue = 0;
		for (i = 0; i < events->numEvents; i++)
		{
			VstEvent* e = events->events[i];
			block = e->deltaFrames - cue;
			if (block > 0) {
				zynayumi.audio_process(p1, p2, block);
				p1 += block;
				p2 += block;
			}
			processEvent(e);
			cue = e->deltaFrames;
		}
	}

	// Process audio
	zynayumi.audio_process(p1, p2, sampleFrames - cue);

	// Release events pointer
	events = nullptr;
}

void VSTZynayumi::processReplacing(float **inputs, float **outputs,
                                   VstInt32 sampleFrames)
{
	for(VstInt32 i = 0; i < sampleFrames; i++) {
		outputs[0][i] = 0.0f;
		outputs[1][i] = 0.0f;
	}
	process(inputs, outputs, sampleFrames);
}

VstIntPtr VSTZynayumi::dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value,
                                  void *ptr, float opt)
{
	int result = 0;

	switch (opCode)
	{

	case effSetSampleRate:  // Set sample rate
		zynayumi.set_sample_rate((int)opt);
		break;
	case effProcessEvents:	// Process events
		events = (VstEvents*)ptr;
		result = 1;
		break;
	default:                // Default 
		result = AudioEffectX::dispatcher(opCode, index, value, ptr, opt);
	}

	return result;
}

void VSTZynayumi::setParameter(VstInt32 index, float value)
{
	// TODO: optimize to avoid systematic getTimeInfo call
	zynayumi.set_bpm(getTimeInfo(kVstTempoValid)->tempo);
	_parameters.set_norm_value((ParameterIndex)index, value);
}

float VSTZynayumi::getParameter(VstInt32 index)
{
	return _parameters.norm_float_value((ParameterIndex)index);
}

void VSTZynayumi::getParameterName(VstInt32 index, char *text)
{
	strcpy(text, _parameters.get_name((ParameterIndex)index).c_str());
}

void VSTZynayumi::getParameterLabel(VstInt32 index, char *text)
{
	strcpy(text, _parameters.get_unit((ParameterIndex)index).c_str());
}

void VSTZynayumi::getParameterDisplay(VstInt32 index, char *text)
{
	strcpy(text, _parameters.get_value_str((ParameterIndex)index).c_str());
}

bool VSTZynayumi::getEffectName(char* name)
{
	strcpy(name, "Zynayumi");
	return true;
}

bool VSTZynayumi::getVendorString(char* text) {
	strcpy(text, "Nil Geisweiller");
	return true;
}

bool VSTZynayumi::getProductString(char* text)
{
	strcpy(text, "Nil Geisweiller Zynayumi");
	return true;
}

VstInt32 VSTZynayumi::getVendorVersion()
{
	return 1;
}

bool VSTZynayumi::getOutputProperties(VstInt32 index,
                                      VstPinProperties* properties)
{
	if(index < 2)
	{
		sprintf(properties->label, "Zynayumi %u", index + 1);
		properties->flags = kVstPinIsActive;
		properties->flags |= kVstPinIsStereo; // Make channel 1+2 stereo
		return true;
	}
	return false;
}

VstInt32 VSTZynayumi::canDo(char* text)
{
	if(!strcmp (text, "receiveVstEvents")) return 1;
	if(!strcmp (text, "receiveVstMidiEvent"))	return 1;
	return -1;
}
