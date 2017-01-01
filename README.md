# Zynayumi

Synth based on ayumi, the highly precise emulator of AY-8910 and
YM2149 http://sovietov.com/app/ayumi/ayumi.html.

## Features

- Amplitude Envelope
- Pitch Envelope
- Portamento
- Vibrato
- Arpeggio (only chord mode for now)
- Ring Modulation (SID and more)
- VST, DSSI
- No GUI

## TODO

- [ ] Expose parameters to the host
- [ ] Support the 3 voices of the YM2149 and map midi channel to them
- [ ] LV2

## Install

1. Fetch ayumi-lib
```bash
$ git submodule init
$ git submodule update
```
2. Create build directory and run cmake
```bash
$ mkdir build
$ cd build
$ cmake ..
```
3. Compile
```bash
$ make
```
4. Install
```bash
$ sudo make install
```

## Author

Nil Geisweiller
