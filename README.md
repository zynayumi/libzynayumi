# Zynayumi

Synth based on ayumi, the highly precise emulator of AY-8910 and
YM2149 http://sovietov.com/app/ayumi/ayumi.html.

## Features

- Arpeggio (not implemented)
- Portamento (not implemented)
- Buzzer (not implemented)
- LFO (not implemented)
- Envelope
- DSSI
- VST

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
