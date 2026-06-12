# SPinSynth-T

SPinSynth-T is a monophonic virtual analog software synthesizer developed by Ricardo Peculis for the Teensy 3.1 microcontroller platform.

This repository preserves and documents the original SPinSynth-T project as a working experimental synthesizer baseline. The project began in 2015 and was restored, cleaned up, documented, and prepared for release in 2026.

## Project Status

SPinSynth-T is not a finished commercial product. It was created to experiment with real-time digital sound synthesis on embedded hardware.

The oscillator, filter, amplifier, envelope generator, LFO, audio buffering, and synthesis signal path were implemented from scratch. SPinSynth-T does not use the PJRC Teensy Audio Library for audio synthesis. It does, however, use PJRC MIDI and USB-MIDI support.

Release 1.0 is intended as a documented, working baseline for future development.

## Main Features

- Monophonic virtual analog synthesizer.
- Custom oscillator with sine, saw, square/pulse, and XTREME waveform modes.
- PolyBLEP correction on saw and pulse-based oscillator paths.
- Custom 24 dB/octave virtual analog low-pass filter.
- Filter cutoff, resonance, envelope amount, LFO, and modulation wheel control.
- Custom ADSR envelope generator used by the filter and amplifier.
- Custom amplifier stage with velocity and volume control.
- MIDI DIN and USB-MIDI control.
- Double-buffered audio path feeding the Teensy 3.1/3.2 12-bit DAC.

## Hardware Target

SPinSynth-T was developed for Teensy 3.1 and has been compiled and tested using the Teensy 3.2/3.1 board selection.

The project uses the Teensy 3.1/3.2 12-bit DAC output and external analog/audio hardware. See the documentation and images in the `docs` folder for circuit and hardware information.

## Control Model

SPinSynth-T does not include its own built-in HMI. Performance and sound parameters are received through MIDI notes, pitch bend, and MIDI Control Change messages over DIN MIDI and USB-MIDI. The restored project was tested using an Arturia KEYLAB25 MIDI controller.

## Building

To compile the sketch:

1. Open `SPinSynth-T/SPinSynth-T.ino` in the Arduino IDE with Teensyduino installed.
2. Select the Teensy board as `Teensy 3.2/3.1`.
3. Select USB Type as `Serial + MIDI`.
4. Compile and upload to the Teensy.

## Documentation

The `docs` folder contains project documentation, schematics, and hardware images:

- [Architecture Description](docs/documentation/SPinSynth-T_Architecture_Description.pdf)
- [MIDI Operation Manual](docs/documentation/SPinSynth-T_MIDI_Operation_Manual.pdf)
- [Original MIDI Controls PDF](docs/documentation/SPinSynth-T-MIDI%20Controls.pdf)
- [Schematics](docs/documentation/SPinSynth-T-schematics.pdf)
- [Hardware Images](docs/images/)

## Demonstration Videos

A collection of video clips demonstrating sounds and waveforms produced by SPinSynth-T is available on YouTube:

https://youtube.com/playlist?list=PLi_8jxD2XyT1Zw34RQF2nZfcWZKqW2X7R&si=N-VGuUinZ5_6-ORg

## Future Work

Possible future development includes:

- Dedicated HMI using rotary encoders and an LCD.
- Migration to Teensy 4.x.
- Further calibration of control ranges and response curves.
- Additional effects and MIDI features.
- A polyphonic architecture based on multiple SPinSynth-T voice instances.

## Author

Developed by Ricardo Peculis.
