# Zynayumi

Synth based on ayumi, the highly precise emulator of AY-8910 and
YM2149 http://sovietov.com/app/ayumi/ayumi.html.

## Features

- [X] Amplitude Envelope
- [X] Pitch Envelope
- [X] Portamento
- [X] Vibrato
- [X] Arpeggio
- [X] Ring Modulation (SID and more)
- [X] VST, DSSI
- [X] Expose parameters to the host (DSSI only for now)
- [ ] GUI (please help if you want one, I am no GUI guy)
- [ ] Define MIDI control for each parameter
- [ ] LV2

## Requirements

- Boost (version 1.54 minimum) http://www.boost.org/
- DSSI SDK http://dssi.sourceforge.net/ (optional)
- VST SDK http://www.steinberg.net/en/company/developers.html (optional)

## Install

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

For VST support, download the VST SDK
(https://www.steinberg.net/en/company/developers.html) and unzip it
directly under the project root directory.

Under GNU/Linux 64-bit, you may also need to comment out a bit of code
in

```
pluginterfaces/vst2.x/aeffect.h
```

like all definitions of VSTCALLBACK except
```
#define VSTCALLBACK
```

## Author

Nil Geisweiller
