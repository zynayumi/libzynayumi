# LibZynayumi

Library for Zynayumi https://github.com/zynayumi/zynayumi

It provides the engine and presets for Zynayumi.

## Features

- [X] Amplitude envelope
- [X] Tone and noise pitch envelope
- [X] Portamento
- [X] Vibrato
- [X] Ring Modulation (SID-like and more)
- [X] Buzzer
- [X] 16-step sequencer for arpegio and other modulation
- [X] MIDI controls assigned to parameters (Modulation, Portamento
      Time, Volume, Pan, Expression and Sustain)

## Requirements

- Boost (version 1.54 minimum) http://www.boost.org/

## Install

0. Clone that repo with the DPL submodule

```bash
$ git clone --recurse-submodules https://github.com/ngeiswei/zynayumi.git
```

then enter the zynayumi folder

```bash
$ cd zynayumi
```

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

## Parameters

- **Emulation mode**:
  - 0: YM2149 (Atari ST)
  - 1: AY-3-8910 (Amdstrad CPC)

- **Cantus mode**:
  - 0: Mono, use the first voice of the YM2149.
  - 1: Unison, use all voices of the YM2149 to play the same note.
  - 2: Poly, alternate between the three voices of the YM2149.

- **Play mode**:
  - 0: Legato, play mono or unison mode in legato mode.
  - 1: Retrig, play mono or unison mode in retrig mode.
  - 2: UpArp, in mono or unison mode, create arpegio with all pressed
       keys, going from lowest to highest pitch.
  - 3: DownArp, like UpArp but goes from higest to lowest pitch.
  - 4: RandArp, like UpArp or DownArp but the pitch is randomly
       selected amongst the pressed keys.

- **Tone reset**: whether the phase of the tone oscillator gets reset
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

- **Env attack time**: attack time of the amplitude envelope.  Ranges
  from 0.0 to 10.0.

- **Env hold1 level**: level of the first hold of the amplitude
  envelope.  Ranges from 0 to 15.

- **Env inter1 time**: time in second to go from the first hold to the
  second hold level of the amplitude envelope.  Ranges from 0.0 to
  10.0.

- **Env hold2 level**: level of the second hold of the amplitude
  envelope.  Ranges from 0 to 15.

- **Env inter2 time**: time in second to go from the second hold to
  the third hold level of the amplitude envelope.  Ranges from 0.0 to
  10.0.

- **Env hold3 level**: level of the third hold of the amplitude
  envelope.  Ranges from 0 to 15.

- **Env decay time**: time in second to go from the third hold to the
  sustain level of the amplitude envelope.  Range from 0.0 to 10.0.

- **Env sustain level**: sustain level of the amplitude envelope.
  Ranges from 0 to 15.
  
- **Env release**: time in second to go from sustain_level to 0.0 of
  the amplitude envelope.  Ranges from 0.0 to 10.0.

- **PitchEnv attack pitch**: attack pitch of the pitch envelope in
  semitone.  Ranges from -96 to +96.

- **PitchEnv time**: time in second to go from attack_pitch to 0 of
  the pitch envelope.  Ranges from 0.0 to 10.0.

- **PitchEnv smoothness**: how smoothly the destination is reached.
  Ranges from 0.0 (linear and sharp) to 1.0 (smooth).

- **RingMod waveform level1 to level8**: can define an 8 points
  waveform.  That normalized waveform is multipled with the YM2149
  square waveform, thus creating a ring modulation.  Ranges from 0 to
  15.

- **RingMod reset**: whether the phase of the ring modulation is
  synchronized with the phase of the square tone when a new on note is
  created.

- **RingMod sync**: whether the ring modulator wave is restarted at
  every tone cycle.

- **RingMod phase**: phase of the ring modulator.  Ranges from 0.0 to
  1.0.

- **RingMod loop**: how the waveform (drawn or provided by the buzzer)
  must be looped.  Options are:
  - 0: Off
  - 1: Forward
  - 2: PingPong

- **RingMod detune**: detune in semitone of the ring modulation
  waveform relative to the square waveform.  The famous SID phaser
  effect can be obtained by setting a value difference than, though
  close to, zero.  Ranges from -0.5 to +0.5.

- **RingMod transpose**: transposition in semitone of the ring
  modulation waveform relative the square waveform.  Ranges from -36
  to +36.

- **RingMod fixed pitch**: fixed pitch of the ring modulation.  Ranges
  from 0.0 to 127.0, corresponding respectively to the lowest and
  highest pitches of the midi standard.  That paramater is also
  affected by *RingMod detune* and *RingMod transpose*.

- **RingMod fixed vs relative**: how much of the ring modulation pitch
  is fixed versus tone pitch.  Ranges from 0.0 (completely fixed) to
  1.0 (completely determined by tone pitch).

- **RingMod depth**: level of depth of ring modulation.  Ranges from 0
  (no ring modulation) to 15 (full ring modulation).

- **Buzzer enabled**: replace software ring modulator by hardward
  envelope (buzzer).  When enabled all attributes (except the
  waveform) from the software ring modulator are transferred to the
  buzzer, with some important limitations, such as innacurate pitch
  and lack of amplitude envelope.

- **Buzzer shape**: oscillator shape of the buzzer.  Only the shapes:
  - 0: DownSaw
  - 1: UpSaw
  are offered here.  The remaining shapes can be obtained by setting
  the *RingMod loop* parameter to *Off* and *PingPong* to respectively
  obtain the remaining shapes.

- **Seq tone pitch 0 to 15**: relative tone pitch in semitone.  Ranges
  from -48 to +48.

- **Seq noise period 0 to 15**: relative noise period.  Ranges from
  -32 to +32.

- **Seq ringmod pitch 0 to 15**: relative ringmod pitch in semitone.  Ranges
  from -48 to +48.

- **Seq ringmod depth 0 to 15**: depth of the ring modulator.  The
  final depth is obtained by multiplying it with *RingMod depth*.
  Ranges from 0 to 15.

- **Seq level 0 to 15**: level of the voice.  The final level is
  obtained by multiplying it with the envelope level.  Ranges from 0
  to 15.

- **Seq tone on 0 to 15**: whether the tone is on or off. Only valid
  during the tone duration time as set by the *Tone time* parameter.

- **Seq noise on 0 to 15**: whether the noise is on or off. Only valid
  during the noise duration time as set by the *Noise time* parameter.

- **Seq mode**: Sequencer mode
  - 0: Off
  - 1: Forward
  - 2: Backward
  - 3: PingPong
  - 4: Random

- **Seq tempo**: tempo in bpm considered to calculate the frequency of
  the sequencer.  Ranges from 30 to 300.

- **Seq host sync**: whether the tempo's host is used instead of *Seq
  tempo*.  Note that the DSSI version cannot access the tempo's host
  which remains fixed at 120 bpm.

- **Seq beat divisor**: sequencer period is measured in beat period
  (bpm / 60) times the fraction seq beat multiplier / seq beat
  divisor.  Ranges from 1 to 64.

- **Seq beat multiplier**: sequencer period is measured in host beat
  period (bpm / 60) times the fraction seq beat multiplier / seq beat
  divisor.  Ranges from 1 to 64.

- **Seq loop**: sequencer loop point.  Ranges from 0 to 15.

- **Seq end**: sequence end point.  Ranges from 0 to 16.

- **LFO shape**: shape of the LFO
  - 0: Sine
  - 1: Triangle
  - 2: DownSaw
  - 3: UpSaw
  - 4: Square
  - 5: Random

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

- **YM channel enabled 0 to 2**: whether the YM channel is enabled.

- **Pan 0 to 2**: panning level of the 3 voices of the YM2149.
  Ranges from 0.0 to 1.0.  Can be to set to hard left, 0.0, hard
  right, 1.0, or any value in between.

- **Gain**: gain coefficient of the output signal.  Ranges from 0.0 to
  2.0.

- **Pitch wheel range**: range [-pw, +pw] of the pitch wheel in
  semitones.  Ranges from 1 to 12.

- **Velocity sensitivity**: ranges from 0.0, no key velocity is taken
  into account, all notes are played at maximum volume, to 1.0,
  velocity is fully taken into account.

- **Ringmod velocity sensitivity**: ranges from 0.0, key velocity has
  no impact on ringmod depth, to 1.0, key velocity has maximum ringmod
  depth defined by the *Ringmod depth* and *Seq ringmod depth*
  parameters.

- **Noise period pitch sensitivity**: ranges from 0.0, noise period is
  unaffected by key pitch, to 1.0, noise period is maximally affected
  by key pitch, meaning the period linearly decreases from its default
  value in C4 when going to the higher keys, and similarily increases
  when going to the lower keys.

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
   the legacy tuning.

3. The envelop (buzzer) of the YM2149 (Atari ST) offers 32 possible
   volume levels instead of 16 for the AY-3-8910 (Amdstrad CPC).

The YM2149 (Atari ST) was mono while the AY-3-8910 (Amstrad CPC) was
stereo, but here stereo is supported in both modes.  One can always
disable stereo with the *Pan0* to *Pan2* parameters.

### Why do I hear a ringing in my ears?

Be careful, chip sounds tend to be rich in harmonics, if you expose
yourself for too long at high volume you risk to loose your hearing
permanently.

Take plenty of sonic rest.  Use low pass filters, earplugs, work at
low volume, take any means necessary to reduce your exposure to
harmful sounds and protect your hearing.

## TODO

- [ ] Fix issue https://github.com/zynayumi/zynayumi/issues/5
- [ ] Implement the modes suggested in
      https://github.com/zynayumi/zynayumi/issues/5#issuecomment-869054663
- [ ] Implement CC:
  - [ ] Volume
  - [ ] Pan
  - [ ] Expression
  - [ ] Release Time
  - [ ] Attack Time
  - [ ] Decay Time
  - [ ] Vibrato Rate
  - [ ] Vibrato Depth
  - [ ] Vibrato Delay
- [ ] Support midi voice receive, Any, 1..16
- [ ] Improve parameter scaling
- [ ] Maybe have unison decrease gain by the proper amount
- [ ] Fix Buzzer reset
- [ ] Fix buzzer sync
- [ ] Have seq ringmod depth also affects buzzer if enabled
- [ ] Add support for digi drums
- [ ] Implement *Oversampling*
- [ ] Fix ringmod phase issue
- [ ] Update tempo when it has changed on the host
- [ ] Define presets

## Author(s)

- Nil Geisweiller

## Contributor(s)

- Teteros
