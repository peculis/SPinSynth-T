//----------------------------------------------------------------------------------
//SPinSynth-T - Monophonic Software Synthesizer Developed by Ricardo Peculis. 
//Last update: 9 June 2026 - Started: 22 Feb 2015
//----------------------------------------------------------------------------------
//To compile this sketch, select the Teensy board as 3.2/3.1 and USB Type as Serial+MIDI.
//----------------------------------------------------------------------------------
//9 June 2026 - Today I am committing this sketch to GitHub as it is compiling and working as when I
//developed it in 2015. I had to reassemble the hardware in a breadboard because I damaged the original
//printed circuit board. It is working now. I tested all the functions, including MIDI and USB-MIDI.
//I used Arturia KEYLAB25 to test the SPinSynth-T (see the list of MIDI Controls below).
//It is important to observe that SPinSynth-T is not a finished product. It was used to experiment the
//development of a software synthesizer using Teensy 3.1 micro-computer. I developed my own software
//for the Oscillators, Filter, Amplifier, Envelope Generators and I did not make use of the Audio Library
//developed by Paul Stoffregen from PJRC. SPinSynth-T, however, uses his MIDI and USB-MIDI.
//----------------------------------------------------------------------------------
//26 April 2026 - Eleven years after the last change to this sketch, I decided to compile it again.
//The reason was to make sure it was still working to become a baseline for further improvements.
//I will include here a history for my own benefit.
//In the Arduino IDE, I selected the Teensy 3.2/3.1 board and USB Type: USB + MIDI (selected in Tools).
//To my surprise, or not, the sketch did not compile. The reasons beat me. There were too many errors
//and warnings. After a few days working on it, I fixed the errors and a few warnings (there are many
//warnings still there). After a successful compiling the sketch did not work as expected. The synth
//played with strange noises. After much investigation, I figured out that the anti-aliasing technique
//I implemented (polyblep with tables) was causing the problem. Initially I removed the anti-aliasing
//mechanism and the weird noise disappeared. 
//----------------------------------------------------------------------------------
//MIDI Controls from Arturia KEYLAB25 (11 Feb 2017)
//PitchBend   - CC#0  - Pitch Bend
//ModWheel   - CC#1  - Modulation Wheel: LFO Amount and Oscillator Frequency
//VOLUME Pot - CC#7  - Master Volume
//Bank 1 P1  - CC#74 - Filter Cutoff Frequency
//Bank 1 P2  - CC#71 - Filter Resonance
//Bank 1 P3  - CC#76 - Filter LFO Frequency
//Bank 1 P4  - CC#77 - Filter LFO Amount
//Bank 1 P5  - CC#93 - Filter LFO Waveshape: SINE, SAW, INVERTED-SAW, SQUARE/PULSE
//Bank 1 P6  - CC#18 - PitchBendWheel Function: Filter CUTOFF, Filter Resonance, LFO_FILTER, LFO_OSCILATOR
//Bank 1 P7  - CC#19 - Filter Envelope Level
//Bank 1 P8  - CC#16 - Oscillator LFO Rate
//Bank 1 P9  - CC#17 - Oscillator LFO Amount
//Bank 1 P10 - CC#91 - Oscillator LFO Waveshape: SINE, SAW, INVERTED-SAW, SQUARE/PULSE
//Bank 2 P1  - CC#35 - Portamento (Glide)
//Bank 2 P2  - CC#36 - NOT USED
//Bank 2 P3  - CC#37 - SAW-X-TACTOR
//Bank 2 P4  - CC#38 - Oscillator PULSE-WIDTH
//Bank 2 P5  - CC#39 - Oscillator Sub-Factor Amount
//Bank 2 P6  - CC#40 - Oscillator Waveshape: SINE, SAW, SQUARE/PULSE, XTREME
//Bank 2 P7  - CC#41 - MASTER TUNE
//Bank 2 P8  - CC#42 - Oscillator SAW Amount
//Bank 2 P9  - CC#43 - Oscillator PULSE Amount
//Bank 2 P10 - CC#44 - Oscillator SUB Amount
//F1         - CC#67 - Filter Attack
//F2         - CC#68 - Filter Decay
//F3         - CC#69 - Filter Sustain
//F4         - CC#70 - Filter Release
//F5         - CC#87 - Amplifier Attack
//F6         - CC#88 - Amplifier Decay
//F7         - CC#89 - Amplifier Sustain
//F8         - CC#90 - Amplifier Release
//F8         - CC#92 - NOT USED
//----------------------------------------------------------------------------------
//23 Apr 2015: Fixed USB MIDI initialisation ( usbMIDI.begin(); was commented out ) 
//----------------------------------------------------------------------------------
//10 Apr 2015: Implemented Tuning LEDs.
//----------------------------------------------------------------------------------
//8 Apr 2015: I had a compilation scare today. This is a safety checkin to make sure 
//I did not lose any recent uncontrolled changes.
//----------------------------------------------------------------------------------
//4 Apr 2015: Implemented master Volume CC#7. Changed Filter Envelop Amount to
//Bank1 Pot P7 (Fader F9 Bank 1 is not used). Changed the default Filter CutOff 
//to max value (0.9, open filter).
//----------------------------------------------------------------------------------
//3 Apr 2015: Started SPinSynth-02 to remove classes and files that are not in use.
///Created SynthTables that contains a number of tables used for Waveform Generation, 
//Anti-Aliasing BLEP correction and MIDI Pitch to Frequency.
//Added Controls for Oscillator LF0 (Bank1: P8 Freq, P9 Amount, P10 Waveform.
//Implemented Master Tuning +/- OneTone - Bank2 P7, 63 = 440Hz.
//-----------------------------------------------------------------------
//2 Apr 2015: Implemented anti-aliasing Blep algorithm as a table. The Oscillator
//(see XtOscillator class) is now more efficient and I did not experience any crashes 
//as reported on 29 March.
//----------------------------------------------------------------------------------
//29 Mar 2015: Implemented Note ON/OFF event management to replicate the behaviour of
//monophonic keyboards where when two notes are pressed simultaneously the higher note
//will play and when the highest key is released the higher key immediately below
//that is still pressed will play. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Amplitude Modulation and the amount of Envelope Filter Cutoff Modulation.
//With these I completed the basic functionality for the SPin Synth. There are improvements
//to be made but from now on is just "putting icing on the cake".
//-----------------------------------------------------------------------
//19 Mar 2015: USB MIDI is now working including Pitch Bend control
//Moved the Note ON LED to the EnvelopeGenerator Class.
//----------------------------------------------------------------------------------
//16 Mar 2015: Changed XTREME waveform to implement Detuned and SubOscillator
//Detuned detunes the a second SAW waveform by +/- 100 Cents (or one semi-tone)
//SubOscillator adds a third SAW waveform with frequency one octave below (P3 Bank2)
//or one fifth above (P5 Bank2). PULSE WIDTH is controlled by P4 Bank 2. The amount 
//of XTREME SAW, PULSE and SUB can be controlled separately by P8, P9 and P10 Bank 2.
//XTREME SAW Amount CC#42, XTREME FACTOR CC#37: P8 and P3 Bank2
//PULSE Amount CC#43, PULSE WIDTH CC#37: P9 and P4 Bank2
//SUB Amount CC#44, SUB FACTOR CC#39: P10 and P5 Bank2
//-----------------------------------------------------------------------
//15 Mar 2015: Implemented XTREME SAW modulated by two LFOs (see XtOscillator class). 
//Still requires tuning, and additional control, but it works.
//----------------------------------------------------------------------------------
//15 Mar 2015: Implemented the Low Frequency Oscillator (LFO) class and applied it to
//control the Oscillator frequency by setting LFO Rate (0.1 to 12.7Hz), amount (0.0 to 1.0)
//and LFO Waveform (SINE, SAW, INVERTED_SAW, SQUARE and PULSE). 
//I am now using the two Control Change (CC) Banks as following:
//Bank1: P1 Filter Cutoff, Pe Filter Resonance, P3 LFO Rate, P4 LFO Amount, P5 LFO Waveform.
//Modulation Wheel (CC#1) is controlling LFO1 Amount to control Oscillator Pitch Modulation.
//Bank1: P6 to P10 are not in use yet.
//Bank1 Slides: Filter ADSR, Amplifier ADSR and Filter ADSR Amount.
//Bank2: P2, P5, P7 AND P10 are not yet in use.
//Bank2: P1 Portamento (Glide), P8 SAW Amount, P9 SQUARE_PULSE Amount
//Bank2: P3 reserved (SAW) XFACTOR, P4 PULSEWIDTH
//----------------------------------------------------------------------------------
//13 Mar 2015: Resolved the "glitch" caused by high processing load. Initially 
//I thought I had improved performance by replacing floating point with fixed
//point calculation, but that was not the case, it got worse, and I reverted
//to floating point. Therefore, the Oscillator produces samples using floating point.
//Introduced a new waveshape option XTREME in addition to the existing ones. XTREME
//will implement all the features of paraphony (for the moment it just mixes SAW and
//SQUARE_PULSE.
//----------------------------------------------------------------------------------
//12 Mar 2015: I start here the final version of my synthesizer based on Teensy 3.1 
//microcontroller, now names as SPinSynth-01. This version starts from everything 
//I have developed so far as Synth-01-Teensy and it is already a fully operational 
//synthesizer with one VA Oscillator, VA Filter, VA Amplifier, Envelope Generator
//and MIDI interface. Now I will start introducing additional features.
//----------------------------------------------------------------------------------

#include "SynthUtilities.h"

#include "Oscillator.h"
// #include "XtOscillator.h"
#include "Filter.h"
#include  "Amplifier.h"
#include <Arduino.h>

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#ifndef TRUE_FALSE
#define TRUE_FALSE
#define FALSE 0
#define TRUE 1
#endif

enum ModWheelFunction {
   CUTOFF,
   RESONANCE,
   LFO_FILTER,
   LFO_OSCILLATOR
 };

 // XtOscillator vaOscillator;
 Oscillator vaOscillator;
 Filter vaFilter;
 Amplifier vaAmplifier;
 
 const float SAMPLE_RATE = 44100.0;
 const int AUDIO_BUFFER_SIZE = 128;
 const int AUDIO_OUTPUT_PIN = A14;
 const int DAC_MIDPOINT = 2048;
 const int DAC_OUTPUT_SHIFT = 5;
 const float MIDI_CC_MAX_VALUE = 127.0;
 const float CENTER_PULSE_WIDTH = 0.5;
 const float PULSE_WIDTH_MOD_DEPTH = 0.85;
 const float SAW_XFACTOR_RANGE_CENTS = 100.0;
 const float SAW_XFACTOR_CENTER_VALUE = 63.5;
 const float OSC_PULSE_WIDTH_CC_DIVISOR = 317.0;
 const float SUB_FACTOR_BASE = 0.5;

 int outputBufferSize = AUDIO_BUFFER_SIZE;
 
 DoubleBuffer oscillatorBuffer;
 DoubleBuffer filterBuffer;
 DoubleBuffer amplifierBuffer;
 
 ModWheelFunction mModWheelFunction = LFO_OSCILLATOR;
 
 int updatePhase = 0;
 
 boolean synchDAC = FALSE;
 int outputSample = 0;
  
 //This is new for Teensy
 
 IntervalTimer updatevaOscillatorTimer;
 IntervalTimer updateControls;

 // Global MIDI controls
 const byte CC_MOD_WHEEL = 1;
 const byte CC_MASTER_VOLUME = 7;
 const byte CC_MASTER_TUNE = 41;
 const byte CC_MOD_WHEEL_FUNCTION = 18;

 // Oscillator MIDI controls
 const byte CC_OSC_LFO_RATE = 16;
 const byte CC_OSC_LFO_AMOUNT = 17;
 const byte CC_OSC_LFO_WAVEFORM = 91;
 const byte CC_OSC_SAW_AMOUNT = 42;
 const byte CC_OSC_SAW_XFACTOR = 37;
 const byte CC_OSC_PULSE_AMOUNT = 43;
 const byte CC_OSC_PULSE_WIDTH = 38;
 const byte CC_OSC_SUB_AMOUNT = 44;
 const byte CC_OSC_SUB_FACTOR = 39;
 const byte CC_OSC_WAVEFORM = 40;
 const byte CC_PORTAMENTO = 35;

 // Filter MIDI controls
 const byte CC_FILTER_CUTOFF = 74;
 const byte CC_FILTER_RESONANCE = 71;
 const byte CC_FILTER_LFO_RATE = 76;
 const byte CC_FILTER_LFO_AMOUNT = 77;
 const byte CC_FILTER_LFO_WAVEFORM = 93;
 const byte CC_FILTER_ATTACK = 73;
 const byte CC_FILTER_DECAY = 75;
 const byte CC_FILTER_SUSTAIN = 79;
 const byte CC_FILTER_RELEASE = 72;
 const byte CC_FILTER_ENVELOPE_LEVEL = 19;

 // Amplifier MIDI controls
 const byte CC_AMP_ATTACK = 80;
 const byte CC_AMP_DECAY = 81;
 const byte CC_AMP_SUSTAIN = 82;
 const byte CC_AMP_RELEASE = 83;

 // LFO waveform value ranges
 const byte LFO_WAVE_SINE_MAX = 16;
 const byte LFO_WAVE_SAW_MAX = 32;
 const byte LFO_WAVE_INVERTED_SAW_MAX = 48;
 const byte LFO_WAVE_SQUARE_MAX = 64;

 // Oscillator waveform value ranges
 const byte OSC_WAVE_SINE_MAX = 20;
 const byte OSC_WAVE_SAW_MIN = 21;
 const byte OSC_WAVE_SAW_MAX = 42;
 const byte OSC_WAVE_SQUARE_MIN = 42;
 const byte OSC_WAVE_SQUARE_MAX = 62;
 const byte OSC_WAVE_VARIABLE_PULSE_MIN = 62;
 const byte OSC_WAVE_VARIABLE_PULSE_MAX = 118;
 const byte OSC_WAVE_XTREME_MIN = 120;

 // Mod wheel function value ranges
 const byte MOD_WHEEL_CUTOFF_MAX = 20;
 const byte MOD_WHEEL_RESONANCE_MIN = 21;
 const byte MOD_WHEEL_RESONANCE_MAX = 42;
 const byte MOD_WHEEL_FILTER_LFO_MIN = 42;
 const byte MOD_WHEEL_FILTER_LFO_MAX = 62;
 const byte MOD_WHEEL_OSC_LFO_MIN = 62;
 const byte MOD_WHEEL_OSC_LFO_MAX = 118;
 const byte MOD_WHEEL_OSC_LFO_HIGH_MIN = 120;

 //---------------- MIDI Callbacks -------------------------------
 void HandleNoteOn(byte channel, byte pitch, byte velocity) { 
  vaOscillator.noteON(int(pitch), int(velocity));
  vaFilter.trigger();
  vaFilter.noteON(int(pitch), int(velocity));
  vaAmplifier.trigger();
  vaAmplifier.noteON(int(pitch), int(velocity));
}

//RP: I created this Callback
void HandleNoteOff(byte channel, byte pitch, byte velocity){
  vaOscillator.noteOFF(int(pitch), int(velocity));
  vaFilter.noteOFF(int(pitch), int(velocity));
  vaAmplifier.noteOFF(int(pitch), int(velocity));
}

void setOscillatorLFOwaveform(byte value) {
  float mValue;
  if(value <= LFO_WAVE_SINE_MAX){
    vaOscillator.setLFOwaveform(LFO_SINE);
  }
  else if((value > LFO_WAVE_SINE_MAX) && (value <= LFO_WAVE_SAW_MAX)){
    vaOscillator.setLFOwaveform(LFO_SAW);
  }
  else if((value > LFO_WAVE_SAW_MAX) && (value <= LFO_WAVE_INVERTED_SAW_MAX)){
    vaOscillator.setLFOwaveform(LFO_INVERTED_SAW);
  }
  else if((value > LFO_WAVE_INVERTED_SAW_MAX) && (value <= LFO_WAVE_SQUARE_MAX)){
    vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
    vaOscillator.setLFOpulsewidth(CENTER_PULSE_WIDTH);
  }
  else if(value > LFO_WAVE_SQUARE_MAX){
    mValue = float(value - LFO_WAVE_SQUARE_MAX) / float(LFO_WAVE_SQUARE_MAX);
    vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
    vaOscillator.setLFOpulsewidth(CENTER_PULSE_WIDTH * (1.0 - PULSE_WIDTH_MOD_DEPTH * mValue));
  }
}

void setFilterLFOwaveform(byte value) {
  float mValue;
  if(value <= LFO_WAVE_SINE_MAX){
    vaFilter.setLFOwaveform(LFO_SINE);
  }
  else if((value > LFO_WAVE_SINE_MAX) && (value <= LFO_WAVE_SAW_MAX)){
    vaFilter.setLFOwaveform(LFO_SAW);
  }
  else if((value > LFO_WAVE_SAW_MAX) && (value <= LFO_WAVE_INVERTED_SAW_MAX)){
    vaFilter.setLFOwaveform(LFO_INVERTED_SAW);
  }
  else if((value > LFO_WAVE_INVERTED_SAW_MAX) && (value <= LFO_WAVE_SQUARE_MAX)){
    vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
    vaFilter.setLFOpulsewidth(CENTER_PULSE_WIDTH);
  }
  else if(value > LFO_WAVE_SQUARE_MAX){
    mValue = float(value - LFO_WAVE_SQUARE_MAX) / float(LFO_WAVE_SQUARE_MAX);
    vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
    vaFilter.setLFOpulsewidth(CENTER_PULSE_WIDTH * (1.0 - PULSE_WIDTH_MOD_DEPTH * mValue));
  }
}

void setOscillatorWaveform(byte value) {
  float mValue;
  if(value <= OSC_WAVE_SINE_MAX){
    vaOscillator.setWaveform(SINE);
  }
  else if((value > OSC_WAVE_SAW_MIN) && (value <= OSC_WAVE_SAW_MAX)){
    vaOscillator.setWaveform(SAW);
  }
  else if((value > OSC_WAVE_SQUARE_MIN) && (value <= OSC_WAVE_SQUARE_MAX)){
    vaOscillator.setWaveform(SQUARE_PULSE);
    vaOscillator.setPulseWidth(CENTER_PULSE_WIDTH);
  }
  else if((value > OSC_WAVE_VARIABLE_PULSE_MIN) && (value <= OSC_WAVE_VARIABLE_PULSE_MAX)){
    mValue = float(value - OSC_WAVE_VARIABLE_PULSE_MIN) / float(OSC_WAVE_VARIABLE_PULSE_MIN);
    vaOscillator.setWaveform(SQUARE_PULSE);
    vaOscillator.setPulseWidth(CENTER_PULSE_WIDTH * (1.0 - PULSE_WIDTH_MOD_DEPTH * mValue));
  }
  else if(value > OSC_WAVE_XTREME_MIN){
    vaOscillator.setWaveform(XTREME);
    vaOscillator.setPulseWidth(CENTER_PULSE_WIDTH);
  }
}

void setModWheelFunction(byte value) {
  if(value <= MOD_WHEEL_CUTOFF_MAX){
    mModWheelFunction = CUTOFF;
  }
  else if((value > MOD_WHEEL_RESONANCE_MIN) && (value <= MOD_WHEEL_RESONANCE_MAX)){
    mModWheelFunction = RESONANCE;
  }
  else if((value > MOD_WHEEL_FILTER_LFO_MIN) && (value <= MOD_WHEEL_FILTER_LFO_MAX)){
    mModWheelFunction = LFO_FILTER;
  }
  else if((value > MOD_WHEEL_OSC_LFO_MIN) && (value <= MOD_WHEEL_OSC_LFO_MAX)){
    mModWheelFunction = LFO_OSCILLATOR;
  }
  else if(value > MOD_WHEEL_OSC_LFO_HIGH_MIN){
    mModWheelFunction = LFO_OSCILLATOR;
  }
}

//RP: I created this Callback - ModWheel is a ControlChange
//ControlChange sends the numeric values of potentiometers and these are associated with
//a Control like Filter Frequency and Resonance, and Envelope Parameters (ADSR).
void HandleControlChange(byte channel, byte number, byte value) { 
  float mValue, pulseW;
  switch (number){
    case CC_MOD_WHEEL: //Modulation Wheel is controlling LFO Amount for Oscillator frequency
      switch (mModWheelFunction){
        case CUTOFF:
          vaFilter.setModCutoff(value);
        break;
        case RESONANCE:
          vaFilter.setDialResonance(value);
        break;
        case LFO_FILTER:
          vaFilter.setLFOamount(value);
        break;
        case LFO_OSCILLATOR:
          vaOscillator.setLFOamount(value);
        break;
      }
      break;
    case CC_MASTER_TUNE: //Master Tuning
      vaOscillator.setTuning(value);
      break;
    case CC_MOD_WHEEL_FUNCTION: // Determines the function of the Pitch Bend Whel
      setModWheelFunction(value);
      break;
    case CC_OSC_LFO_RATE:
      vaOscillator.setLFOrate(value);
      break;
    case CC_OSC_LFO_AMOUNT:
      vaOscillator.setLFOamount(value);
      break;
    case CC_OSC_LFO_WAVEFORM:
      setOscillatorLFOwaveform(value);
      break;
    case CC_OSC_SAW_AMOUNT:
      mValue = float(value) / MIDI_CC_MAX_VALUE;
      vaOscillator.setSawAmount(mValue);
      break;
     case CC_OSC_SAW_XFACTOR:
      mValue = SAW_XFACTOR_RANGE_CENTS * (-1.0 + float(value) / SAW_XFACTOR_CENTER_VALUE);
      vaOscillator.setSawXfactor(mValue);
      break;
    case CC_OSC_PULSE_AMOUNT:
      mValue = float(value) / MIDI_CC_MAX_VALUE;
      vaOscillator.setPulseAmount(mValue);
      break;
    case CC_OSC_PULSE_WIDTH:
      pulseW = CENTER_PULSE_WIDTH - float(value) / OSC_PULSE_WIDTH_CC_DIVISOR;
      vaOscillator.setPulseWidth(pulseW);
      break;
    case CC_OSC_SUB_AMOUNT:
      mValue = float(value) / MIDI_CC_MAX_VALUE;
      vaOscillator.setSubAmout(mValue);
      break;
    case CC_OSC_SUB_FACTOR:
      mValue = SUB_FACTOR_BASE + float(value) / MIDI_CC_MAX_VALUE;
      vaOscillator.setSubFactor(mValue);
      break;
    case CC_FILTER_CUTOFF:
      vaFilter.setDialCutoff(value);
      break;
    case CC_FILTER_RESONANCE:
      vaFilter.setDialResonance(value);
      break;
    case CC_FILTER_LFO_RATE:
      vaFilter.setLFOrate(value);
      break;
    case CC_FILTER_LFO_AMOUNT:
      vaFilter.setLFOamount(value);
      break;
    case CC_FILTER_LFO_WAVEFORM:
      setFilterLFOwaveform(value);
      break;
    case CC_FILTER_ATTACK:
      vaFilter.setAttack(value);
      break;
    case CC_FILTER_DECAY:
      vaFilter.setDecay(value);
      break;
    case CC_FILTER_SUSTAIN:
      vaFilter.setSustain(value);
      break;
    case CC_FILTER_RELEASE:
      vaFilter.setRelease(value);
      break;
    case CC_FILTER_ENVELOPE_LEVEL:
      vaFilter.setEnvelopeLevel(value);
      break;
    case CC_AMP_ATTACK:
      vaAmplifier.setAttack(value);
      break;
    case CC_AMP_DECAY:
      vaAmplifier.setDecay(value);
      break;
    case CC_AMP_SUSTAIN:
      vaAmplifier.setSustain(value);
      break;
    case CC_AMP_RELEASE:
      vaAmplifier.setRelease(value);
      break;
    case CC_MASTER_VOLUME:
      vaAmplifier.setVolume(value);
      break;
    case CC_OSC_WAVEFORM:
      setOscillatorWaveform(value);
      break;
    case CC_PORTAMENTO:
      vaOscillator.setPortamento(int(value));
      break;  
    default:
      break;
  }
}

//RP: I created this Callback
void HandlePitchBend(byte channel, int bend) { 
  vaOscillator.setPitchBend(bend);
}
  
//----------------- End MIDI Callbacks -------------------------

 // outputBufferedDAC() sends data from the outputBuffer to the DAC.
 //The outputBuffer is filled by the Oscillator with AUDIO_BUFFER_SIZE samples
 //in one call to vaOscillator.getNextBuffer(buffer, bufferSize).
 void outputBufferedDAC(){
   long lAmplifierSample;
   int dacOutput;
   outputSample++;
   if(outputSample == AUDIO_BUFFER_SIZE){
     oscillatorBuffer.swapBuffer();
     filterBuffer.swapBuffer();
     outputSample = 0;
     synchDAC = TRUE;
   }
   lAmplifierSample = amplifierBuffer.read();
   dacOutput = DAC_MIDPOINT + (lAmplifierSample >> DAC_OUTPUT_SHIFT);

   analogWrite(AUDIO_OUTPUT_PIN, dacOutput);
 }

void update(){
  if(filterBuffer.isReadyToWrite() && updatePhase == 1){
    vaFilter.updateBuffer(oscillatorBuffer, filterBuffer);
    vaAmplifier.updateBuffer(filterBuffer, amplifierBuffer);
    updatePhase = 0;
  }
  if(oscillatorBuffer.isReadyToWrite() && synchDAC){
    vaOscillator.updateBuffer(oscillatorBuffer);
    updatePhase = 1;
    synchDAC = FALSE;
  }
  vaOscillator.update();
  vaFilter.update();
  vaAmplifier.update();
}

void controlUpdate() {
  update();
}

void setup() {
  Serial.begin(9600);
  analogWriteResolution(12);
  
  outputBufferSize = AUDIO_BUFFER_SIZE;
  synchDAC = FALSE;
  updatePhase = 0;
  
  pinMode(LED, OUTPUT);
  pinMode(TUNED, OUTPUT);
  pinMode(FLAT, OUTPUT);
  pinMode(SHARP, OUTPUT);
  oscillatorBuffer.start(outputBufferSize);
  filterBuffer.start(outputBufferSize);
  amplifierBuffer.start(outputBufferSize);
 
  //Initialise Teensy's IntervalTimers
  //Buffered output to DAC: update is called every 22.6 µseconds (44.1KHz rate)
  updatevaOscillatorTimer.begin(outputBufferedDAC, 1000000.0 / SAMPLE_RATE); //Parameters are: Function to be called and interval is in µseconds
  updatevaOscillatorTimer.priority(10);
  updateControls.begin(controlUpdate, 250.0); //Controls are updated at 250.0µseconds or 0.25mseconds
  updateControls.priority(128);
  //MIDI Setup
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);    
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleControlChange(HandleControlChange);
  MIDI.setHandlePitchBend(HandlePitchBend); 
  
  usbMIDI.begin();    
  usbMIDI.setHandleNoteOn(HandleNoteOn);
  usbMIDI.setHandleNoteOff(HandleNoteOff);
  usbMIDI.setHandleControlChange(HandleControlChange);
  usbMIDI.setHandlePitchChange(HandlePitchBend); 
}

void loop(){

  MIDI.read();
  usbMIDI.read();
 }
