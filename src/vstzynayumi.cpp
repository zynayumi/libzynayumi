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

#include "vstzynayumi.hpp"
#include "parameters.hpp"

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
	: AudioEffectX(audioMaster, 1, PARAMETERS_COUNT), events(nullptr)
	, _tone_detune(0)
	, _tone_transpose(0)
	, _ringmode_detune(0)
	, _ringmode_transpose(0)
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
		case CTL_PORTAMENTO_TIME: {
			float valuef = affine(0, 127, 0.0f, 1.0f, value);
			setParameterAutomated(PORTAMENTO, valuef);
			break;
		}
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
		std::cerr << "effSetSampleRate not implemented" << std::endl;
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
	switch(index) {
		// Play mode
	case PLAY_MODE:
		zynayumi.patch.playmode =
			(PlayMode)std::round(value * (float)PlayMode::RndArp);
		break;

		// Tone
	case TONE_TIME:
		zynayumi.patch.tone.time =
			affine(0.0f, 1.0f, TONE_TIME_MIN, TONE_TIME_MAX, value);
		break;
	case TONE_DETUNE:
		_tone_detune =
			affine(0.0f, 1.0f, TONE_DETUNE_MIN, TONE_DETUNE_MAX, value);
		zynayumi.patch.tone.detune = _tone_detune + _tone_transpose;
		break;
	case TONE_TRANSPOSE:
		_tone_transpose =
			(int)std::round(affine(0.0f, 1.0f,
			                       TONE_TRANSPOSE_MIN, TONE_TRANSPOSE_MAX,
			                       value));
		zynayumi.patch.tone.detune = _tone_detune + _tone_transpose;
		break;

		// Noise
	case NOISE_TIME:
		zynayumi.patch.noise.time =
			affine(0.0f, 1.0f, NOISE_TIME_MIN, NOISE_TIME_MAX, value);
		break;
	case NOISE_PERIOD:
		zynayumi.patch.noise.period =
			(int)std::round(affine(0.0f, 1.0f,
			                       NOISE_PERIOD_MIN, NOISE_PERIOD_MAX,
			                       value));
		break;

		// Noise period envelope
	case NOISE_PERIOD_ENV_ATTACK:
		zynayumi.patch.noise_period_env.attack =
			(int)std::round(affine(0.0f, 1.0f,
			                       NOISE_PERIOD_ENV_ATTACK_MIN,
			                       NOISE_PERIOD_ENV_ATTACK_MAX,
			                       value));;
		break;
	case NOISE_PERIOD_ENV_TIME:
		zynayumi.patch.noise_period_env.time =
			affine(0.0f, 1.0f,
			       NOISE_PERIOD_ENV_TIME_MIN,
			       NOISE_PERIOD_ENV_TIME_MAX, value);
		break;

		// Amplitude envelope
	case AMP_ENV_ATTACK_LEVEL:
		zynayumi.patch.ampenv.attack_level = value;
		break;
	case AMP_ENV_TIME1:
		zynayumi.patch.ampenv.time1 =
			affine(0.0f, 1.0f, AMP_ENV_TIME1_MIN, AMP_ENV_TIME1_MAX, value);
		break;
	case AMP_ENV_LEVEL1:
		zynayumi.patch.ampenv.level1 = value;
		break;
	case AMP_ENV_TIME2:
		zynayumi.patch.ampenv.time2 =
			affine(0.0f, 1.0f, AMP_ENV_TIME2_MIN, AMP_ENV_TIME2_MAX, value);
		break;
	case AMP_ENV_LEVEL2:
		zynayumi.patch.ampenv.level2 = value;
		break;
	case AMP_ENV_TIME3:
		zynayumi.patch.ampenv.time3 =
			affine(0.0f, 1.0f, AMP_ENV_TIME3_MIN, AMP_ENV_TIME3_MAX, value);
		break;
	case AMP_ENV_SUSTAIN_LEVEL:
		zynayumi.patch.ampenv.sustain_level = value;
		break;
	case AMP_ENV_RELEASE:
		zynayumi.patch.ampenv.release =
			affine(0.0f, 1.0f, AMP_ENV_RELEASE_MIN, AMP_ENV_RELEASE_MAX, value);
		break;

		// Pitch envelope
	case PITCH_ENV_ATTACK_PITCH:
		zynayumi.patch.pitchenv.attack_pitch =
			affine(0.0f, 1.0f,
			       PITCH_ENV_ATTACK_PITCH_MIN, PITCH_ENV_ATTACK_PITCH_MAX, value);
		break;
	case PITCH_ENV_TIME:
		zynayumi.patch.pitchenv.time =
			affine(0.0f, 1.0f, PITCH_ENV_TIME_MIN, PITCH_ENV_TIME_MAX, value);
		break;

		// Arpegio
	case ARP_PITCH1:
		zynayumi.patch.arp.pitch1 =
			affine(0.0f, 1.0f, ARP_PITCH1_MIN, ARP_PITCH1_MAX, value);
		break;
	case ARP_PITCH2:
		zynayumi.patch.arp.pitch2 =
			affine(0.0f, 1.0f, ARP_PITCH2_MIN, ARP_PITCH2_MAX, value);
		break;
	case ARP_PITCH3:
		zynayumi.patch.arp.pitch3 =
			affine(0.0f, 1.0f, ARP_PITCH3_MIN, ARP_PITCH3_MAX, value);
		break;
	case ARP_FREQ:
		zynayumi.patch.arp.freq =
			affine(0.0f, 1.0f, ARP_FREQ_MIN, ARP_FREQ_MAX, value);
		break;
	case ARP_REPEAT:
		zynayumi.patch.arp.repeat =
			(int)std::round(affine(0.0f, 1.0f,
			                       ARP_REPEAT_MIN, ARP_REPEAT_MAX, value));
		break;

		// Ring modulation
	case RING_MOD_WAVEFORM_LEVEL1:
		zynayumi.patch.ringmod.waveform[0] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL2:
		zynayumi.patch.ringmod.waveform[1] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL3:
		zynayumi.patch.ringmod.waveform[2] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL4:
		zynayumi.patch.ringmod.waveform[3] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL5:
		zynayumi.patch.ringmod.waveform[4] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL6:
		zynayumi.patch.ringmod.waveform[5] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL7:
		zynayumi.patch.ringmod.waveform[6] = value;
		break;
	case RING_MOD_WAVEFORM_LEVEL8:
		zynayumi.patch.ringmod.waveform[7] = value;
		break;
	case RING_MOD_MIRROR:
		zynayumi.patch.ringmod.mirror = (bool)std::round(value);
		break;
	case RING_MOD_SYNC:
		zynayumi.patch.ringmod.sync = (bool)std::round(value);
		break;
	case RING_MOD_DETUNE:
		_ringmode_detune =
			affine(0.0f, 1.0f, RING_MOD_DETUNE_MIN, RING_MOD_DETUNE_MAX, value);
		zynayumi.patch.ringmod.detune = _ringmode_detune + _ringmode_transpose;
		break;
	case RING_MOD_TRANSPOSE:
		_ringmode_transpose =
			(int)std::round(affine(0.0f, 1.0f,
			                       RING_MOD_TRANSPOSE_MIN, RING_MOD_TRANSPOSE_MAX,
			                       value));
		zynayumi.patch.ringmod.detune = _ringmode_detune + _ringmode_transpose;
		break;

		// Pitch LFO
	case LFO_FREQ:
		zynayumi.patch.lfo.freq =
			affine(0.0f, 1.0f, LFO_FREQ_MIN, LFO_FREQ_MAX, value);
		break;
	case LFO_DELAY:
		zynayumi.patch.lfo.delay =
			affine(0.0f, 1.0f, LFO_DELAY_MIN, LFO_DELAY_MAX, value);
		break;
	case LFO_DEPTH:
		zynayumi.patch.lfo.depth =
			affine(0.0f, 1.0f, LFO_DEPTH_MIN, LFO_DEPTH_MAX, value);
		break;

		// Portamento
	case PORTAMENTO:
		zynayumi.patch.port =
			affine(0.0f, 1.0f, PORTAMENTO_MIN, PORTAMENTO_MAX, value);
		break;

		// Pan
	case PAN_CHANNEL0:
		zynayumi.patch.pan.channel[0] = value;
		break;
	case PAN_CHANNEL1:
		zynayumi.patch.pan.channel[1] = value;
		break;
	case PAN_CHANNEL2:
		zynayumi.patch.pan.channel[2] = value;
		break;

		// Pitch wheel range
	case PITCH_WHEEL:
		zynayumi.patch.pitchwheel =
			affine(0.0f, 1.0f, PITCH_WHEEL_MIN, PITCH_WHEEL_MAX, value);
		break;

		// Emulation mode
	case EMUL_MODE:
		zynayumi.patch.emulmode =
			(EmulMode)std::round(value * (float)EmulMode::AY8910);
		break;

	default:
		std::cout << "setParameter(" << index << ", " << value << ")"
		          << std::endl;
	}
}

float VSTZynayumi::getParameter(VstInt32 index)
{
	switch(index) {
		// Play mode
	case PLAY_MODE:
		return (float)zynayumi.patch.playmode / (float)PlayMode::RndArp;

		// Tone
	case TONE_TIME:
		return affine(TONE_TIME_MIN, TONE_TIME_MAX,
		              0.0f, 1.0f, zynayumi.patch.tone.time);
	case TONE_DETUNE:
		return affine(TONE_DETUNE_MIN, TONE_DETUNE_MAX,
		              0.0f, 1.0f, _tone_detune);
	case TONE_TRANSPOSE:
		return affine(TONE_TRANSPOSE_MIN, TONE_TRANSPOSE_MAX,
		              0.0f, 1.0f, (float)_tone_transpose);

		// Noise
	case NOISE_TIME:
		return affine(NOISE_TIME_MIN, NOISE_TIME_MAX,
		              0.0f, 1.0f, zynayumi.patch.noise.time);
	case NOISE_PERIOD:
		return affine(NOISE_PERIOD_MIN, NOISE_PERIOD_MAX,
		              0.0f, 1.0f, zynayumi.patch.noise.period);

		// Noise period envelope
	case NOISE_PERIOD_ENV_ATTACK:
		return affine(NOISE_PERIOD_ENV_ATTACK_MIN,
		              NOISE_PERIOD_ENV_ATTACK_MAX,
		              0.0f, 1.0f,
		              zynayumi.patch.noise_period_env.attack);
		break;
	case NOISE_PERIOD_ENV_TIME:
		return affine(NOISE_PERIOD_ENV_TIME_MIN,
		              NOISE_PERIOD_ENV_TIME_MAX,
		              0.0f, 1.0f,
		              zynayumi.patch.noise_period_env.time);
		break;

		// Amplitude envelope
	case AMP_ENV_ATTACK_LEVEL:
		return zynayumi.patch.ampenv.attack_level;
	case AMP_ENV_TIME1:
		return affine(AMP_ENV_TIME1_MIN, AMP_ENV_TIME1_MAX,
		              0.0f, 1.0f, zynayumi.patch.ampenv.time1);
	case AMP_ENV_LEVEL1:
		return zynayumi.patch.ampenv.level1;
	case AMP_ENV_TIME2:
		return affine(AMP_ENV_TIME2_MIN, AMP_ENV_TIME2_MAX,
		              0.0f, 1.0f, zynayumi.patch.ampenv.time2);
	case AMP_ENV_LEVEL2:
		return zynayumi.patch.ampenv.level2;
	case AMP_ENV_TIME3:
		return affine(AMP_ENV_TIME3_MIN, AMP_ENV_TIME3_MAX,
		              0.0f, 1.0f, zynayumi.patch.ampenv.time3);
	case AMP_ENV_SUSTAIN_LEVEL:
		return zynayumi.patch.ampenv.sustain_level;
	case AMP_ENV_RELEASE:
		return affine(AMP_ENV_RELEASE_MIN, AMP_ENV_RELEASE_MAX,
		              0.0f, 1.0f, zynayumi.patch.ampenv.release);

		// Pitch envelope
	case PITCH_ENV_ATTACK_PITCH:
		return affine(PITCH_ENV_ATTACK_PITCH_MIN, PITCH_ENV_ATTACK_PITCH_MAX,
		              0.0f, 1.0f, zynayumi.patch.pitchenv.attack_pitch);
	case PITCH_ENV_TIME:
		return affine(PITCH_ENV_TIME_MIN, PITCH_ENV_TIME_MAX,
		              0.0f, 1.0f, zynayumi.patch.pitchenv.time);

		// Arpegio
	case ARP_PITCH1:
		return affine(ARP_PITCH1_MIN, ARP_PITCH1_MAX,
		              0.0f, 1.0f, zynayumi.patch.arp.pitch1);
	case ARP_PITCH2:
		return affine(ARP_PITCH2_MIN, ARP_PITCH2_MAX,
		              0.0f, 1.0f, zynayumi.patch.arp.pitch2);
	case ARP_PITCH3:
		return affine(ARP_PITCH3_MIN, ARP_PITCH3_MAX,
		              0.0f, 1.0f, zynayumi.patch.arp.pitch3);
	case ARP_FREQ:
		return affine(ARP_FREQ_MIN, ARP_FREQ_MAX,
		              0.0f, 1.0f, zynayumi.patch.arp.freq);
	case ARP_REPEAT:
		return affine(ARP_REPEAT_MIN, ARP_REPEAT_MAX,
		              0.0f, 1.0f, (float)zynayumi.patch.arp.repeat);

		// Ring modulation
	case RING_MOD_WAVEFORM_LEVEL1:
		return zynayumi.patch.ringmod.waveform[0];
	case RING_MOD_WAVEFORM_LEVEL2:
		return zynayumi.patch.ringmod.waveform[1];
	case RING_MOD_WAVEFORM_LEVEL3:
		return zynayumi.patch.ringmod.waveform[2];
	case RING_MOD_WAVEFORM_LEVEL4:
		return zynayumi.patch.ringmod.waveform[3];
	case RING_MOD_WAVEFORM_LEVEL5:
		return zynayumi.patch.ringmod.waveform[4];
	case RING_MOD_WAVEFORM_LEVEL6:
		return zynayumi.patch.ringmod.waveform[5];
	case RING_MOD_WAVEFORM_LEVEL7:
		return zynayumi.patch.ringmod.waveform[6];
	case RING_MOD_WAVEFORM_LEVEL8:
		return zynayumi.patch.ringmod.waveform[7];
	case RING_MOD_MIRROR:
		return (float)zynayumi.patch.ringmod.mirror;
	case RING_MOD_SYNC:
		return (float)zynayumi.patch.ringmod.sync;
	case RING_MOD_DETUNE:
		return affine(RING_MOD_DETUNE_MIN, RING_MOD_DETUNE_MAX,
		              0.0f, 1.0f, _ringmode_detune);
	case RING_MOD_TRANSPOSE:
		return affine(RING_MOD_TRANSPOSE_MIN, RING_MOD_TRANSPOSE_MAX,
		              0.0f, 1.0f, (float)_ringmode_transpose);

		// Pitch LFO
	case LFO_FREQ:
		return affine(LFO_FREQ_MIN, LFO_FREQ_MAX,
		              0.0f, 1.0f, zynayumi.patch.lfo.freq);
	case LFO_DELAY:
		return affine(LFO_DELAY_MIN, LFO_DELAY_MAX,
		              0.0f, 1.0f, zynayumi.patch.lfo.delay);
	case LFO_DEPTH:
		return affine(LFO_DEPTH_MIN, LFO_DEPTH_MAX,
		              0.0f, 1.0f, zynayumi.patch.lfo.depth);

		// Portamento
	case PORTAMENTO:
		return affine(PORTAMENTO_MIN, PORTAMENTO_MAX,
		              0.0f, 1.0f, zynayumi.patch.port);

		// Pan
	case PAN_CHANNEL0:
		return zynayumi.patch.pan.channel[0];
	case PAN_CHANNEL1:
		return zynayumi.patch.pan.channel[1];
	case PAN_CHANNEL2:
		return zynayumi.patch.pan.channel[2];

		// Pitch wheel range
	case PITCH_WHEEL:
		return affine(PITCH_WHEEL_MIN, PITCH_WHEEL_MAX,
		              0.0f, 1.0f, (double)zynayumi.patch.pitchwheel);

		// Emulation mode
	case EMUL_MODE:
		return (float)zynayumi.patch.emulmode / (float)EmulMode::AY8910;

	default:
		return 0.0f;
	}
}

void VSTZynayumi::getParameterLabel(VstInt32 index, char *text)
{
	strcpy(text, "unit");
}

void VSTZynayumi::getParameterName(VstInt32 index, char *text)
{
	switch(index) {
		// Play mode
	case PLAY_MODE:
		strcpy(text, PLAY_MODE_STR);
		break;

		// Tone
	case TONE_TIME:
		strcpy(text, TONE_TIME_STR);
		break;
	case TONE_DETUNE:
		strcpy(text, TONE_DETUNE_STR);
		break;
	case TONE_TRANSPOSE:
		strcpy(text, TONE_TRANSPOSE_STR);
		break;

		// Noise
	case NOISE_TIME:
		strcpy(text, NOISE_TIME_STR);
		break;
	case NOISE_PERIOD:
		strcpy(text, NOISE_PERIOD_STR);
		break;

		// Noise period envelope
	case NOISE_PERIOD_ENV_ATTACK:
		strcpy(text, NOISE_PERIOD_ENV_ATTACK_STR);
		break;
	case NOISE_PERIOD_ENV_TIME:
		strcpy(text, NOISE_PERIOD_ENV_TIME_STR);
		break;

		// Amplitude envelope
	case AMP_ENV_ATTACK_LEVEL:
		strcpy(text, AMP_ENV_ATTACK_LEVEL_STR);
		break;
	case AMP_ENV_TIME1:
		strcpy(text, AMP_ENV_TIME1_STR);
		break;
	case AMP_ENV_LEVEL1:
		strcpy(text, AMP_ENV_LEVEL1_STR);
		break;
	case AMP_ENV_TIME2:
		strcpy(text, AMP_ENV_TIME2_STR);
		break;
	case AMP_ENV_LEVEL2:
		strcpy(text, AMP_ENV_LEVEL2_STR);
		break;
	case AMP_ENV_TIME3:
		strcpy(text, AMP_ENV_TIME3_STR);
		break;
	case AMP_ENV_SUSTAIN_LEVEL:
		strcpy(text, AMP_ENV_SUSTAIN_LEVEL_STR);
		break;
	case AMP_ENV_RELEASE:
		strcpy(text, AMP_ENV_RELEASE_STR);
		break;

		// Pitch envelope
	case PITCH_ENV_ATTACK_PITCH:
		strcpy(text, PITCH_ENV_ATTACK_PITCH_STR);
		break;
	case PITCH_ENV_TIME:
		strcpy(text, PITCH_ENV_TIME_STR);
		break;

		// Arpegio
	case ARP_PITCH1:
		strcpy(text, ARP_PITCH1_STR);
		break;
	case ARP_PITCH2:
		strcpy(text, ARP_PITCH2_STR);
		break;
	case ARP_PITCH3:
		strcpy(text, ARP_PITCH3_STR);
		break;
	case ARP_FREQ:
		strcpy(text, ARP_FREQ_STR);
		break;
	case ARP_REPEAT:
		strcpy(text, ARP_REPEAT_STR);
		break;

		// Ring modulation
	case RING_MOD_WAVEFORM_LEVEL1:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL1_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL2:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL2_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL3:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL3_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL4:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL4_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL5:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL5_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL6:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL6_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL7:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL7_STR);
		break;
	case RING_MOD_WAVEFORM_LEVEL8:
		strcpy(text, RING_MOD_WAVEFORM_LEVEL8_STR);
		break;
	case RING_MOD_MIRROR:
		strcpy(text, RING_MOD_MIRROR_STR);
		break;
	case RING_MOD_SYNC:
		strcpy(text, RING_MOD_SYNC_STR);
		break;
	case RING_MOD_DETUNE:
		strcpy(text, RING_MOD_DETUNE_STR);
		break;
	case RING_MOD_TRANSPOSE:
		strcpy(text, RING_MOD_TRANSPOSE_STR);
		break;

		// Pitch LFO
	case LFO_FREQ:
		strcpy(text, LFO_FREQ_STR);
		break;
	case LFO_DELAY:
		strcpy(text, LFO_DELAY_STR);
		break;
	case LFO_DEPTH:
		strcpy(text, LFO_DEPTH_STR);
		break;

		// Portamento
	case PORTAMENTO:
		strcpy(text, PORTAMENTO_STR);
		break;

		// Pan
	case PAN_CHANNEL0:
		strcpy(text, PAN_CHANNEL0_STR);
		break;
	case PAN_CHANNEL1:
		strcpy(text, PAN_CHANNEL1_STR);
		break;
	case PAN_CHANNEL2:
		strcpy(text, PAN_CHANNEL2_STR);
		break;

		// Pitch wheel range
	case PITCH_WHEEL:
		strcpy(text, PITCH_WHEEL_STR);
		break;

		// Emulation mode
	case EMUL_MODE:
		strcpy(text, EMUL_MODE_STR);
		break;

	default:
		strcpy(text, "no parameter");
	}
}

void VSTZynayumi::getParameterDisplay(VstInt32 index, char *text)
{
	switch(index) {
		// Play mode
	case PLAY_MODE:
		switch(zynayumi.patch.playmode) {
		case PlayMode::Mono: strcpy(text, "Mono");
			break;
		case PlayMode::Poly: strcpy(text, "Poly");
			break;
		case PlayMode::UpArp: strcpy(text, "UpArp");
			break;
		case PlayMode::DownArp: strcpy(text, "DownArp");
			break;
		case PlayMode::RndArp: strcpy(text, "RndArp");
			break;
		default: strcpy(text, "no display");
		}
		break;

		// Tone
	case TONE_TIME:
		strcpy(text, std::to_string(zynayumi.patch.tone.time).c_str());
		break;
	case TONE_DETUNE:
		strcpy(text, std::to_string(_tone_detune).c_str());
		break;
	case TONE_TRANSPOSE:
		strcpy(text, std::to_string(_tone_transpose).c_str());
		break;

		// Noise
	case NOISE_TIME:
		strcpy(text, std::to_string(zynayumi.patch.noise.time).c_str());
		break;
	case NOISE_PERIOD:
		strcpy(text, std::to_string(zynayumi.patch.noise.period).c_str());
		break;

		// Noise period envelope
	case NOISE_PERIOD_ENV_ATTACK:
		strcpy(text, std::to_string(zynayumi.patch.noise_period_env.attack).c_str());
		break;
	case NOISE_PERIOD_ENV_TIME:
		strcpy(text, std::to_string(zynayumi.patch.noise_period_env.time).c_str());
		break;

		// Amplitude envelope
	case AMP_ENV_ATTACK_LEVEL:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.attack_level).c_str());
		break;
	case AMP_ENV_TIME1:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.time1).c_str());
		break;
	case AMP_ENV_LEVEL1:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.level1).c_str());
		break;
	case AMP_ENV_TIME2:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.time2).c_str());
		break;
	case AMP_ENV_LEVEL2:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.level2).c_str());
		break;
	case AMP_ENV_TIME3:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.time3).c_str());
		break;
	case AMP_ENV_SUSTAIN_LEVEL:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.sustain_level).c_str());
		break;
	case AMP_ENV_RELEASE:
		strcpy(text, std::to_string(zynayumi.patch.ampenv.release).c_str());
		break;

		// Pitch envelope
	case PITCH_ENV_ATTACK_PITCH:
		strcpy(text, std::to_string(zynayumi.patch.pitchenv.attack_pitch).c_str());
		break;
	case PITCH_ENV_TIME:
		strcpy(text, std::to_string(zynayumi.patch.pitchenv.time).c_str());
		break;

		// Arpegio
	case ARP_PITCH1:
		strcpy(text, std::to_string(zynayumi.patch.arp.pitch1).c_str());
		break;
	case ARP_PITCH2:
		strcpy(text, std::to_string(zynayumi.patch.arp.pitch2).c_str());
		break;
	case ARP_PITCH3:
		strcpy(text, std::to_string(zynayumi.patch.arp.pitch3).c_str());
		break;
	case ARP_FREQ:
		strcpy(text, std::to_string(zynayumi.patch.arp.freq).c_str());
		break;
	case ARP_REPEAT:
		strcpy(text, std::to_string(zynayumi.patch.arp.repeat).c_str());
		break;

		// Ring modulation
	case RING_MOD_WAVEFORM_LEVEL1:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[0]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL2:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[1]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL3:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[2]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL4:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[3]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL5:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[4]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL6:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[5]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL7:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[6]).c_str());
		break;
	case RING_MOD_WAVEFORM_LEVEL8:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.waveform[7]).c_str());
		break;
	case RING_MOD_MIRROR:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.mirror).c_str());
		break;
	case RING_MOD_SYNC:
		strcpy(text, std::to_string(zynayumi.patch.ringmod.sync).c_str());
		break;
	case RING_MOD_DETUNE:
		strcpy(text, std::to_string(_ringmode_detune).c_str());
		break;
	case RING_MOD_TRANSPOSE:
		strcpy(text, std::to_string(_ringmode_transpose).c_str());
		break;

		// Pitch LFO
	case LFO_FREQ:
		strcpy(text, std::to_string(zynayumi.patch.lfo.freq).c_str());
		break;
	case LFO_DELAY:
		strcpy(text, std::to_string(zynayumi.patch.lfo.delay).c_str());
		break;
	case LFO_DEPTH:
		strcpy(text, std::to_string(zynayumi.patch.lfo.depth).c_str());
		break;

		// Portamento
	case PORTAMENTO:
		strcpy(text, std::to_string(zynayumi.patch.port).c_str());
		break;

		// Pan
	case PAN_CHANNEL0:
		strcpy(text, std::to_string(zynayumi.patch.pan.channel[0]).c_str());
		break;
	case PAN_CHANNEL1:
		strcpy(text, std::to_string(zynayumi.patch.pan.channel[1]).c_str());
		break;
	case PAN_CHANNEL2:
		strcpy(text, std::to_string(zynayumi.patch.pan.channel[2]).c_str());
		break;

		// Pitch wheel range
	case PITCH_WHEEL:
		strcpy(text, std::to_string(zynayumi.patch.pitchwheel).c_str());
		break;

		// Emulation mode
	case EMUL_MODE:
		switch(zynayumi.patch.emulmode) {
		case EmulMode::YM2149: strcpy(text, "YM2149");
			break;
		case EmulMode::AY8910: strcpy(text, "AY-3-8910");
			break;
		default: strcpy(text, "no display");
			break;
		}
		break;

	default:
		strcpy(text, "no display");
	}
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
