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
- [X] Buzzer
- [X] VST, DSSI
- [X] MIDI controls assigned to parameters (Modulation, Portamento
      Time, Volume, Pan, Expression and Sustain).
- [ ] LV2 (meanwhile you can use [NASPRO](http://naspro.sourceforge.net/plugins.html#naspro-bridges))
- [ ] GUI.  Please help if you want one, I am no GUI guy.

## Demo Songs

* [Monstrosity by A-Lin](https://lbry.tv/@ngeiswei:d/A-Lin---Monstrosity---2020-06-09:f)

If you make a song with Zynayumi let me know so I can include it.

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

Just install the DSSI SDK.  You likely have a package in your
distribution, otherwise, have a look at http://dssi.sourceforge.net/.

Note that the DSSI version has been left behind a little bit.  The VST
version is more complete as of today.

## VST Support

For VST2 support, you need a copy of the old VST36 SDK as recent ones don't come with VST2 headers anymore.

[vstsdk367_03_03_2017_build_352](https://www.steinberg.net/sdk_downloads/vstsdk367_03_03_2017_build_352.zip) is the last known one still available.

Just unzip the VST2_SDK folder to the Zynayumi root folder.

Under GNU/Linux 64-bit, you may need to do this patch first:

```diff
--- VST2_SDK/pluginterfaces/vst2.x/aeffect.h
+++ VST2_SDK/pluginterfaces/vst2.x/aeffect.h
@@ -66,7 +66,11 @@
        #pragma options push -a8
 #elif defined(__GNUC__)
     #pragma pack(push,8)
-    #define VSTCALLBACK __cdecl
+    #if defined(__linux__)
+        #define VSTCALLBACK
+    #else
+        #define VSTCALLBACK __cdecl
+    #endif
 #elif defined(WIN32) || defined(__FLAT__) || defined CBUILDER
        #pragma pack(push)
        #pragma pack(8)
```

## LV2 Support

LV2 is not supported yet.  If for some reason you can only compile the
DSSI version and your host does not support DSSI, such as
[Ardour](https://ardour.org/), you can use
[NASPRO](http://naspro.sourceforge.net/plugins.html#naspro-bridges)
which will expose Zynayumi as if it were an LV2 plugin.  It is not
completely stable however, it can crash the DAW during initialization,
but if it starts then it should run flawlessly.

## Parameters

- **Emulation mode**:
  - 0: YM2149 (Atari ST)
  - 1: AY-3-8910 (Amdstrad CPC)

- **Play mode**:
  - 0: Mono, always use the first voice of the YM2149.
  - 1: MonoUpArp, create arpegio with all pressed keys, going from
       lowest to highest pitch.
  - 2: MonoDownArp, like UpArp but goes from higest to lowest pitch.
  - 3: MonoRandArp, like UpArp or DownArp but the pitch is randomly
       selected amongst the pressed keys.
  - 4: Unison, all voices play the same note.
  - 5: UnisonUpArp, like MonoUpArp with all voices in unison.
  - 6: UnisonDownArp, like MonoDownArp with all voices in unison.
  - 7: UnisonRandArp, like MonoRandArp with all voices in unison.
  - 8: Poly, alternate between the three voices of the YM2149.

- **Tone sync**: whether the phase of the tone oscillator gets reset
  to *Tone phase* when a new on note is created.

- **Tone phase**: phase of the tone oscillator.  Ranges from 0.0 to
  1.0.

- **Tone time**: time in second the square voice is played.  Ranges
  from 0.0 to +inf.

- **Tone detune**: global detune in semitone.  Ranges from -0.5 to
  +0.5.

- **Tone transpose**: global transpose in semitone.  Ranges from -24
  to +24.

- **Tone Spread** detune the second and third voices respectively by
  minus and plus the spread expressed in semintone. Ranges from 0.0 to
  0.5.

- **Tone legacy tuning**: whether the pitch is perfect or follows the
  legacy tuning.

- **Noise time**: time the noise generator is played in second.
  Ranges from 0.0 to +inf.

- **Noise period**: affect the frequency of the noise.  Ranges from 1
  to 31.

- **NoisePeriodEnv attack**: attack period of the noise.  Ranges from
  1 to 31.

- **NoisePeriodEnv time**: time in second to go from noise period
  attack to noise attack.  Ranges from 0.0 to 5.0.

- **Env attack time**: attack time of the envelope.  Ranges from 0.0
  to 10.0.

- **Env hold1 level**: level of the first hold of the envelope.
  Ranges from 0.0 to 1.0.

- **Env inter1 time**: time in second to go from the first hold to the
  second hold level of the envelope.  Ranges from 0.0 to 10.0.

- **Env hold2 level**: level of the second hold of the envelope.
  Ranges from 0.0 to 1.0.

- **Env inter2 time**: time in second to go from the second hold to
  the third hold level of the envelope.  Ranges from 0.0 to 10.0.

- **Env hold3 level**: level of the third hold of the envelope.
  Ranges from 0.0 to 1.0.

- **Env decay time**: time in second to go from the third hold to the
  sustain level of the envelope.  Range from 0.0 to 10.0.

- **Env sustain level**: sustain level of the envelope.  Ranges from
  0.0 to 1.0.
  
- **Env release**: time in second to go from sustain_level to 0.0 of
  the envelope.  Ranges from 0.0 to 10.0.
  
- **Amp env depth**: Depth of the amplitude envelope.  Ranges from 0.0
  (full amplitude) to 1.0 (envelope amplitude).

- **PitchEnv attack pitch**: attack pitch of the pitch envelope in
  semitone.  Ranges from -96 to +96.

- **PitchEnv time**: time in second to go from attack_pitch to 0 of
  the pitch envelope.  Ranges from 0.0 to 10.0.

- **Arp pitch1**: pitch in semitone of the first arpegio note.  Only
  active for play mode 0 (Mono) and 1 (Poly).  Ranges from -48 to +48.

- **Arp pitch2**: pitch in semitone of the second arpegio note.  Only
  active for play mode 0 (Mono) and 1 (Poly).  Ranges from -48 to +48.

- **Arp pitch3**: pitch in semitone of the third arpegio note.  Only
  active for play mode 0 (Mono) and 1 (Poly).  Ranges from -48 to +48.

- **Arp tempo**: tempo in bpm considered to calculate the frequency of
  the arpegio.  Ranges from 30 to 300.

- **Arp host sync**: whether the tempo's host is used instead of *Arp
  tempo*.  Note that the DSSI version cannot access the tempo's host
  which remains fixed at 120 bpm.

- **Arp beat divisor**: arpegio period is measured in beat period (bpm
  / 60) times the fraction arp beat multiplier / arp beat divisor.
  Ranges from 1 to 64.

- **Arp beat multiplier**: arpegio period is measured in host beat
  period (bpm / 60) times the fraction arp beat multiplier / arp beat
  divisor.  Ranges from 1 to 64.

- **Arp repeat**: select which pitch to repeat the arpegio.
  - 0: from pitch1, thus cycle through pitch1 to pitch3.
  - 1: from pitch2, thus cycle through pitch2 to pitch3.
  - 2: from pitch3, thus cycle through pitch3, that is no arpegio
    expect for the first cycle.

- **RingMod waveform level1 to level8**: can define an 8 points
  waveform.  That waveform is multipled with the YM2149 square
  waveform, thus creating a ring modulation.  Ranges from 0.0 to 1.0.

- **RingMod sync**: whether the phase of the ring modulation is
  synchronized with the phase of the square tone when a new on note is
  created.

- **RingMod phase**: phase of the ring modulator.  Ranges from 0.0 to
  1.0.

- **RingMod mirror**: whether the ring modulation waveform is
  mirrored.  This can soften the ring modulation.

- **RingMod detune**: detune in semitone of the ring modulation
  waveform relative to the square waveform.  The famous SID phaser
  effect can be obtained by setting a value difference than, though
  close to, zero.  Ranges from -0.5 to +0.5.

- **RingMod transpose**: transposition in semitone of the ring
  modulation waveform relative the square waveform.  Ranges from -36
  to +36.

- **RingMod fixed frequency**: fixed frequency of the ring modulation.
  Ranges from 1Hz to 5000Hz.

- **RingMode fixed vs relative**: how much of the ring modulation
  frequency is fixed versus tone pitch.  Ranges from 0.0 (completely
  fixed) to 1.0 (completely determined by tone pitch).

- **RingMod depth**: level of depth of ring modulation.  Ranges from
  0.0 (no ring modulation) to 1.0 (full ring modulation).

- **RingMod env depth**: level of ring modulation envelope depth.
  Ranges from -1.0 (maximum ring modulation when the envelope is low),
  0.0 (no ring modulation envelope), to 1.0 (maximum ring modulation
  when the envelope is high).

- **Buzzer shape**: oscillator shape of the buzzer.  Warning: the
  buzzer disactivates the envelope and the ring modulator.
  - 0: DownSaw
  - 1: DownTriangle
  - 2: UpSaw
  - 3: UpTriangle
  
- **Buzzer sync**: whether the phase of the buzzer gets reset to
  *Buzzer phase* when a new on note is created.

- **Buzzer phase**: phase of the buzzer.  Ranges from 0.0 to 1.0.

- **Buzzer time**: buzzer duration in second.  Ranges from 0.0 to
  +inf.

- **Buzzer detune**: buzzer detune in semitone.  Ranges from -0.5 to
  +0.5.

- **Buzzer transpose**: buzzer transpose in semitone.  Ranges from
  -36 to +36.

- **LFO shape**: shape of the LFO
  - 0: Sine
  - 1: Triangle
  - 2: DownSaw
  - 3: UpSaw
  - 4: Square
  - 5: Rand

- **LFO freq**: frequency of the low frequency oscillation to create a
  vibrato effect.  Ranges from 0.0 to 20.0.

- **LFO delay**: time in second to progressively reach full LFO depth.
  Ranges from 0.0 to 10.0.

- **LFO depth**: LFO depth in semitone.  Ranges from 0.0 to 12.0.

- **Portamento time**: time in second of the portamento effect,
  constant regardless of the distance between notes.  Ranges from 0.0
  to 2.0.

- **Portamento smoothness**: determine how linear vs sigmoidal the
  portamento pitch curve is.  Ranges from 0.0 (linear) to 1.0
  (sigmoidal).

- **Gain**: gain coefficient of the output signal.  Ranges from 0.0 to
  2.0.

- **Pan0 to Pan2**: panning level of the 3 voices of the YM2149.
  Ranges from 0.0 to 1.0.  Can be to set to hard left, 0.0, hard
  right, 1.0, or any value in between.

- **Pitch wheel range**: range [-pw, +pw] of the pitch wheel in
  semitones.  Ranges from 1 to 12.
  
- **Velocity sensitivity**: ranges from 0.0, no key velocity is taken
  into account, all notes are played at maximum volume, to 1.0,
  velocity is fully taken into account.

- **Modulation sensitivity**: ranges from 0.0, modulation wheel is
  ignored, to 12.0 modulation wheel is fully taken into account for
  pitch LFO up to 12 semitones.

## MIDI Controls

### Control Changes (CC)

Control changes range from 0 to 127.

- 1: **Modulation** [default=0]: set the modulation depth up to the
     value in semitone defined by the *Modulation sensitivity*
     parameter.
- 5: **Portamento Time** [default=0]: set portamento time in second
     from 0 to 2, added to the *Portamento time* parameter.
- 7: **Volume** [default=100]: control gain *g=v^2/127^2*, multiplied by
     the gains of *Expression* CC and *Gain* parameter.
- 10: **Pan** [default=64]: panning control.
- 11: **Expression** [default=127]: control gain *g=v^2/127^2*,
      multiplied by the gains of *Volume* CC and *Gain* parameter.
- 64: **Sustain Pedal** [default=0]: *v<64* is off, *64<=v* is on.
- 72: **Release Time** [default=64]: *v<64* is shorter, *64<v* is longer. [TODO]
- 73: **Attack Time** [default=64]: *v<64* is shorter, *64<v* is longer. [TODO]
- 75: **Decay Time** [default=64]: *v<64* is shorter, *64<v* is longer. [TODO]
- 76: **Vibrato Rate** [default=64]: *v<64* is slower, *64<v* is faster. [TODO]
- 77: **Vibrato Depth** [default=64]: *v<64* is shallower, *64<v* is deeper. [TODO]
- 78: **Vibrato Delay** [default=64]: *v<64* is shorter, *64<v* is longer. [TODO]
- 123: **All Sound Off**: set all voices off.

### Pitch wheel

The pitch wheel [default=8192] ranges from 0 to 16383, corresponds to
`-pw` to `+pw` in semitone where `pw` is the value set by the plugin
parameter *Pitch wheel range*.

## FAQ

### Are these clicks and glitches normal?

It's easy to generate undesired clicks, especially in the attack and
release.  This may not be a bug of Zynayumi though.  To avoid such
clicks set a non null attack time and release of the amplitude
envelope.  See *Env attack time* and *Env release* parameters.

Also, the amplitude envelope itself can create clicks as it is driving
the emulated YM2149 to control the amplitude in a staircase fashion.
This is a feature, not a bug.

### Wait, that tuning is too perfect to come from the YM2149!?

Yes, I've modified ayumi so that tuning is perfect.  To enable legacy
tuning instead use the *Tone legacy tuning* parameter.  Using legacy
tuning produces more glitches when combined with the ring modulator.
This is a feature, not a bug.

### Why does the buzzer disable the envelop?

Because the buzzer was originally designed to be an envelop, even
though it is typically used as an oscillator, as is the case here.  To
avoid disabling the envelop you may use the ring modulator instead.

### Why can't I hear any difference between the YM2149 and AY-3-8910?

The difference is subtle and has to do with

1. The mapping of the 16 possible volume levels to their corresponding
   dBs.

2. The frequency of the clock, 2MHz for the YM2149 (Atari ST) and 1MHz
   for AY-3-8910 (Amdstrad CPC).  This affects the noise period and
   the legacy mode tuning.

The YM2149 (Atari ST) was mono while the AY-3-8910 (Amstrad CPC) was
stereo, but here stereo is supported in both modes.  One can always
disable stereo with the *Pan0* to *Pan2* parameters.

### Why do I hear a ringing in my ears?

Be careful, chip sounds tend to be rich in harmonics, if you expose
yourself for too long at high volume you risk to loose your hearing
permanently.

Take plenty of sonic rest.  Use low pass filters, ear plugs, work at
low volume, take any means necessary to reduce your exposure to
harmful sounds and protect your hearing.

## TODO

- [ ] Add amp and ringmod env sensitivity parameter
- [ ] Add 8-step sequencer for controlling arp, noise and vol
- [ ] Fix ringmod phase issue
- [ ] Update tempo when it has changed on the host
- [ ] Implement pitch wheel and CC for DSSI
- [ ] Implement CC:
  - [ ] Release Time
  - [ ] Attack Time
  - [ ] Decay Time
  - [ ] Vibrato Rate
  - [ ] Vibrato Depth
  - [ ] Vibrato Delay
- [ ] Define presets
- [ ] Add LV2 support

## Author

Nil Geisweiller

## Contributor(s)

Teteros
