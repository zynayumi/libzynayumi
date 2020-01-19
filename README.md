# Zynayumi

Synth based on ayumi, the highly precise emulator of AY-8910 and
YM2149 http://sovietov.com/app/ayumi/ayumi.html.

## Features

- [X] Amplitude Envelope
- [X] Tone and Noise Pitch Envelope
- [X] Portamento
- [X] Vibrato
- [X] Arpeggio
- [X] Ring Modulation (SID and more)
- [X] VST, DSSI
- [ ] Buzzer
- [ ] LV2 (meanwhile you can use [NASPRO](http://naspro.sourceforge.net/plugins.html#naspro-bridges))
- [ ] MIDI controls assigned to parameters (only portamento for now)
- [ ] GUI. Pease help if you want one, I am no GUI guy

## Requirements

- Boost (version 1.54 minimum) http://www.boost.org/
- DSSI SDK http://dssi.sourceforge.net/ (optional)
- VST SDK http://www.steinberg.net/en/company/developers.html (optional)

## Installation

1. Create build directory and run cmake
```bash
$ mkdir build
$ cd build
$ cmake ..
```
2. Compile
```bash
$ make
```
3. Install
```bash
$ sudo make install
```

## DSSI Support

Just install the DSSI SDK. You likely have a package in your
distribution, otherwise, have a look at http://dssi.sourceforge.net/.

## VST Support

For VST support, you need an old DSK as recent ones no longer support
VST2. You may find one
[here](https://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip). If
the link no longer works let me know, hopefully I have a copy of it.

Under GNU/Linux 64-bit, you may need to comment out some code in

```
pluginterfaces/vst2.x/aeffect.h
```

specifically all definitions of `VSTCALLBACK` except the last one

```
#define VSTCALLBACK
```

## LV2 Support

LV2 is not supported yet. If for some reason you can only compile the
DSSI version and your host does not support DSSI, such as
[Ardour](https://ardour.org/), you can use
[NASPRO](http://naspro.sourceforge.net/plugins.html#naspro-bridges)
which will expose Zynayumi as if it were an LV2 plugin. It is not
completely stable however, it can crash the DAW during initialization,
but if it starts then it should run flawlessly.

## Parameters Description

- **Play mode:**
  - 0: Mono, always use the first voice of the YM2149.
  - 1: Poly, alternate between the three voices of the YM2149.
  - 2: UpArp, create arpegio with all pressed keys, going from lowest
       to highest pitch.
  - 3: DownArp, like UpArp but goes from higest to lowest pitch.
  - 4: RndArp, like UpArp or DownArp but the pitch is randomly
       selected amongst the pressed keys.

- **Tone time:** time in second the square voice is played. Ranges
  from 0.0 to +inf.

- **Tone detune:** global detune in semitone. Ranges from -0.5 to
  +0.5.

- **Tone transpose:** global transpose in semitone. Ranges from -24 to
  +24.

- **Noise time:** time the noise generator is played in second. Ranges
  from 0.0 to +inf.

- **Noise period:** affect the frequency of the noise. Ranges from 1 to
  31.

- **NoisePeriodEnv attack:** attack period of the noise. Ranges from 1 to
  31.

- **NoisePeriodEnv time:** time in second to go from noise period attack
  to noise attack. Ranges from 0.0 to 5.0.

- **AmpEnv attack level:** attack level of the amplitude envelope. Ranges
  from 0.0 to 1.0.

- **AmpEnv time1:** time in second to go from attack level to level1 of
  the amplitude envelope. Ranges from 0.0 to 5.0.

- **AmpEnv level1:** first intermediary level of the amplitude
  envelope. Ranges from 0.0 to 1.0.

- **AmpEnv time2:** time in second to go from level1 to level2 of the
  amplitude envelope. Ranges from 0.0 to 5.0.

- **AmpEnv level2:** second intermediary level of the amplitude
  envelope. Ranges from 0.0 to 1.0.

- **AmpEnv time3:** time in second to go from level2 to sustain_level of
  the amplitude envelope. Range from 0.0 to 5.0.

- **AmpEnv sustain level:** sustain level of the amplitude
  envelope. Ranges from 0.0 to 1.0.
  
- **AmpEnv release:** time in second to go from sustain_level to 0.0 of
  the amplitude envelope. Ranges from 0.0 to 5.0.

- **PitchEnv attack pitch:** attack pitch of the pitch envelope in
  semitone. Ranges from -96 to +96.

- **PitchEnv time:** time in second to go from attack_pitch to 0 of the
  pitch envelope. Ranges from 0.0 to 5.0.

- **Arp pitch1:** pitch in semitone of the first arpegio note. Only active
  for play mode 0 (Mono) and 1 (Poly). Ranges from -48 to +48.

- **Arp pitch2:** pitch in semitone of the second arpegio note. Only
  active for play mode 0 (Mono) and 1 (Poly). Ranges from -48 to +48.

- **Arp pitch3:** pitch in semitone of the third arpegio note. Only active
  for play mode 0 (Mono) and 1 (Poly). Ranges from -48 to +48.

- **Arp freq:** frequency of the arpegio pitch change. For instance if its
  value is 1.0, the arpegio will change the pitch every second. Ranges
  from 0.0 to 50.0.

- **Arp repeat:** select which pitch to repeat the arpegio.
  - 0: from pitch1, thus cycle through pitch1 to pitch3.
  - 1: from pitch2, thus cycle through pitch2 to pitch3.
  - 2: from pitch3, thus cycle through pitch3, that is no arpegio
    expect for the first cycle.

- **RingMod waveform level1 to level8:** can define an 8 points
  waveform. That waveform is multipled with the YN2149 square
  waveform, thus creating a ring modulation. Ranges from 0.0 to 1.0.

- **RingMod mirror:** whether the ring modulation waveform is
  mirrored. This can soften the ring modulation.

- **RingMod sync:** whether the phase of the ring modulation is
  synchronized with the phase of the square tone when a new on note is
  created.

- **RingMod detune:** detune in semitone of the ring modulation waveform
  relative to the square waveform. The famous SID phaser effect can be
  obtained by setting a value difference than, though close to,
  zero. Ranges from -0.5 to +0.5.

- **RingMod transpose:** transposition in semitone of the ring modulation
  waveform relative the square waveform. Ranges from -24 to +24.

- **LFO freq:** frequency of the low frequency oscillation to create a
  vibrato effect. Ranges from 0.0 to 20.0.

- **LFO delay:** time in second to progressively reach full LFO
  depth. Ranges from 0.0 to 10.0.

- **LFO depth:** LFO depth in semitone. Ranges from 0.0 to 12.0.

- **Portamento time:** time in second of the portamento effect to go from 1
  semitone to the next. Ranges from 0.0 to 0.5.

- **Pan channel0 to channel2:** panning level of the 3 voices of the
  YM2149. Ranges from 0.0 to 1.0. Can be to set to hard left, 0.0,
  hard right, 1.0, or any value in between.

- **Pitch wheel range:** range [-pw, +pw] of the pitch wheel in
  semitones. Ranges from 1 to 12.

- **Emulation mode:**
  - 0: YM2149 (Atari ST)
  - 1: AY-3-8910 (Amdstrad CPC, ZX Spectrum)

## Clicks and other glitches

It's easy to generate undesired clicks, especially in the attack and
release. This may not be a bug of Zynayumi though. To avoid such
clicks set a non null release of the amplitude envelope. See AmpEnv
release in the section above.

Also, the amplitude envelope itself can create clicks as it is using
the YM2149 to control the amplitude in a staircase fashion. This is a
feature, not a bug.

## Wait, that tuning is too perfect to come from the YM2149! 

Yes, I've modified ayumi so that tuning is perfect, even in high
pitch. I prefer it that way. A parameter could be added that lets the
user choose between legacy and perfect tuning. I may not add it soon,
but if you do your contribution is welcome.

## TODO

- [ ] Add velocity sensitivity parameter
- [ ] Add modulation (LFO) sensitivity parameter
- [ ] Fix click in release (maybe?)
- [ ] Define presets
- [ ] Map parameters to MIDI CC
- [ ] Support Buzzer (envelope)
- [ ] Add parameter to choose between perfect and legacy tuning
- [ ] Add LV2 support

## Author

Nil Geisweiller
