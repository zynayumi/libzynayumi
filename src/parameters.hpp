/****************************************************************************
    
    Parameters for Zynayumi

    parameters.hpp

    Copyleft (c) 2016-2019 Nil Geisweiller <ngeiswei@gmail.com>
 
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

#ifndef __ZYNAYUMI_PARAMETERS_HPP
#define __ZYNAYUMI_PARAMETERS_HPP

namespace zynayumi {

static enum {
	// Play mode
	PLAY_MODE,

	// Tone
	TONE_TIME,
	TONE_DETUNE,
	TONE_TRANSPOSE,

	// Noise
	NOISE_TIME,
	NOISE_PERIOD,

	// Amplitude envelope
	AMP_ENV_ATTACK_LEVEL,
	AMP_ENV_TIME1,
	AMP_ENV_LEVEL1,
	AMP_ENV_TIME2,
	AMP_ENV_LEVEL2,
	AMP_ENV_TIME3,
	AMP_ENV_SUSTAIN_LEVEL,
	AMP_ENV_RELEASE,

	// Pitch envelope
	PITCH_ENV_ATTACK_PITCH,
	PITCH_ENV_TIME,

	// Arpegio
	ARP_PITCH1,
	ARP_PITCH2,
	ARP_PITCH3,
	ARP_FREQ,
	ARP_REPEAT,

	// Ring modulation
	RING_MOD_WAVEFORM_LEVEL1,
	RING_MOD_WAVEFORM_LEVEL2,
	RING_MOD_WAVEFORM_LEVEL3,
	RING_MOD_WAVEFORM_LEVEL4,
	RING_MOD_WAVEFORM_LEVEL5,
	RING_MOD_WAVEFORM_LEVEL6,
	RING_MOD_WAVEFORM_LEVEL7,
	RING_MOD_WAVEFORM_LEVEL8,
	RING_MOD_MIRROR,
	RING_MOD_SYNC,
	RING_MOD_DETUNE,
	RING_MOD_TRANSPOSE,

	// Pitch LFO
	LFO_FREQ,
	LFO_DELAY,
	LFO_DEPTH,

	// Portamento
	PORTAMENTO,

	// Pan
	PAN_CHANNEL0,
	PAN_CHANNEL1,
	PAN_CHANNEL2,

	// Number of Parameters
	PARAMETERS_COUNT
} parameters;

// Parameter names
#define PLAY_MODE_STR "Play mode"
#define TONE_TIME_STR "Tone time"
#define TONE_DETUNE_STR "Tone detune"
#define TONE_TRANSPOSE_STR "Tone transpose"
#define NOISE_TIME_STR "Noise time"
#define NOISE_PERIOD_STR "Noise period"
#define AMP_ENV_ATTACK_LEVEL_STR "AmpEnv attack_level"
#define AMP_ENV_TIME1_STR "AmpEnv time1"
#define AMP_ENV_LEVEL1_STR "AmpEnv level1"
#define AMP_ENV_TIME2_STR "AmpEnv time2"
#define AMP_ENV_LEVEL2_STR "AmpEnv level2"
#define AMP_ENV_TIME3_STR "AmpEnv time3"
#define AMP_ENV_SUSTAIN_LEVEL_STR "AmpEnv sustain_level"
#define AMP_ENV_RELEASE_STR "AmpEnv release"
#define PITCH_ENV_ATTACK_PITCH_STR "PitchEnv attack_pitch"
#define PITCH_ENV_TIME_STR "PitchEnv time"
#define ARP_PITCH1_STR "Arp pitch1"
#define ARP_PITCH2_STR "Arp pitch2"
#define ARP_PITCH3_STR "Arp pitch3"
#define ARP_FREQ_STR "Arp freq"
#define ARP_REPEAT_STR "Arp repeat"
#define RING_MOD_WAVEFORM_LEVEL1_STR "RingMod waveform level1"
#define RING_MOD_WAVEFORM_LEVEL2_STR "RingMod waveform level2"
#define RING_MOD_WAVEFORM_LEVEL3_STR "RingMod waveform level3"
#define RING_MOD_WAVEFORM_LEVEL4_STR "RingMod waveform level4"
#define RING_MOD_WAVEFORM_LEVEL5_STR "RingMod waveform level5"
#define RING_MOD_WAVEFORM_LEVEL6_STR "RingMod waveform level6"
#define RING_MOD_WAVEFORM_LEVEL7_STR "RingMod waveform level7"
#define RING_MOD_WAVEFORM_LEVEL8_STR "RingMod waveform level8"
#define RING_MOD_MIRROR_STR "RingMod mirror"
#define RING_MOD_SYNC_STR "RingMod sync"
#define RING_MOD_DETUNE_STR "RingMod detune"
#define RING_MOD_TRANSPOSE_STR "RingMod transpose"
#define LFO_FREQ_STR "LFO freq"
#define LFO_DELAY_STR "LFO delay"
#define LFO_DEPTH_STR "LFO depth"
#define PORTAMENTO_STR "Portamento"
#define PAN_CHANNEL0_STR "Pan channel0"
#define PAN_CHANNEL1_STR "Pan channel1"
#define PAN_CHANNEL2_STR "Pan channel2"

// Affine transformation, based on the equality
//
// (y - miny) / (maxy - miny) = (x - minx) / (maxx - minx)
// y = ((x - minx) / (maxx - minx)) * (maxy - miny) + miny
static float affine(float minx, float maxx, float miny, float maxy, float x) {
	return ((x - minx) / (maxx - minx)) * (maxy - miny) + miny;
}

} // ~namespace zynayumi

#endif
