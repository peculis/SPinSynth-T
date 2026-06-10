//----------------------------------------------------------------------------------
//SPinSynth-T - Monophonic Software Synthesizer Developed by Ricardo Peculis. 
//Last update: 9 June 2026 - Started: 22 Feb 2015
//----------------------------------------------------------------------------------
//To compile this scketch, select the Teensy board as 3.2/3.1 and USB Type as Serial+MIDI.
//----------------------------------------------------------------------------------
//9 June 2026 - Today I am commiting this sketch to GitHub as it is compilinh and working as when I
//developed it in 2015. I had to reassemble the hardware in a breadboard because I damaged the original
//printed cricuit board. It is working now. I tested all the functions, including MIDI and USB-MIDI.
//I used Artyuria KEYLAB25 to test the SPinSynth-T (see the list of MIDI Controls below).
//It is important to observe that SPinSynth-T is not a finished product. It was used to experiment the
//development of a software synthesizer using Teensy 3.1 micro-computer. I developed my own software
//for the Oscilators, Filter, Amplifier, Envelope Generatos and I didi not make use of the Audio Library
//developed by Paul Stoffregen from PJRC. SPinSynth-T, however, uses his MIDI and USB-MIDI.
//----------------------------------------------------------------------------------
//26 April 2026 - Eleven years after the last change to this sketch, I decided to compile it again.
//The reason was to make sure it was still working to become a basline for futher improvements.
//I will include here a history for my own benefit.
//In the Arduini Ide, I selected the Teeansy 3.2/3.1 board and USB Type: USB + MIDI (selected in Tools).
//To my surprise, or not, the sketch did not compile. The reasons beat me. There were too many errors
//and warnings. After a few days working on it, I fixed the errors and a few warnings (there are many
//warnings still there). After a successful compiling the sketch did not work as expected. The synth
//played with strange noises. After much investigation, I figured out that the anti-aliasing technique
//I implemented (polyblep with tables) was causing the problem. Initially I removed the anti-aliasing
//mechanism and the weird noise disapeared. 
//----------------------------------------------------------------------------------
//MIDI Controls from Arturia KEYLAB25 (11 Feb 2017)
//PichBend   - CC#0  - Pitch Bend
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
//F1         - CC#67 - Filter Atack
//F2         - CC#68 - Filter Decay
//F3         - CC#69 - Filter Sustain
//F4         - CC#70 - Filter Release
//F5         - CC#87 - Amplifier Atack
//F6         - CC#88 - Amplifier Decay
//F7         - CC#89 - Amplifier Sustain
//F8         - CC#90 - Amplifier Release
//F8         - CC#92 - NOT USED
//----------------------------------------------------------------------------------
//23 Apr 2015: Fixed USB MIDI initialisation ( usbMIDI.begin(); was commented out ) 
//----------------------------------------------------------------------------------
//10 Apr 2015: Implemened Tuning LEDs.
//----------------------------------------------------------------------------------
//8 Apr 2015: I had a compilation scare today. This is a safety checkin to make sure 
//I did not lose any recent uncontrolled changes.
//----------------------------------------------------------------------------------
//4 Apr 2015: Implemented master Volume CC#7. Changed Filter Envelop Amout to
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
//monophonic keyboards where when two notes are presses simultaneously the higher note
//will play and when the highest key is released the heigher key immediately below
//that is still pressed will play. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Amplitude Modulation and the amount of Envelope Filter Cutoff Modulation.
//With these I completed the basic functionality for the SPin Synth. There are improvements
//to be made but from now on is just "putting icing on the cake".
//-----------------------------------------------------------------------
//19 Mar 2015: USB MIDI is now working including Pitch Bend control
//Moved the Note ON LED to the EnvelopeGenerator Class.
//----------------------------------------------------------------------------------
//16 Mar 2015: Changed XTREME waveform to implement Detuned and SubOscillator
//Detuned detunes the a second SAW wavefor by +/- 100 Cents (or one semi-tone)
//SubOscillator adds a third SAW waveform with frequency one octave below (P3 Bank2)
//or one fifth above (P5 Bank2). PULSE WIDTH is controlled by P4 Bank 2. The amount 
//of XTREME SAW, PULSE and SUB can be controlled separatelly by P8, P9 and P10 Bank 2.
//XTREME SAW Amout CC#42, XTREME FACTOR CC#37: P8 and P3 Bank2
//PULSE Amount CC#43, PULSE WIDTH CC#37: P9 and P4 Bank2
//SUB Amount CC#44, SUB FACTOR CC#39: P10 and P5 Bank2
//-----------------------------------------------------------------------
//15 Mar 2015: Implemented XTREME SAW modulated by two LFOs (see XtOscillator class). 
//Still requires tuning, and additional control, but it works.
//----------------------------------------------------------------------------------
//15 Mar 2015: Implemented the Low Frequency Oscillator (LFO) class and applied it to
//control the Oscillator frequency by seeting LFO Rate (0.1 to 12.7Hz), amout (0.0 to 1.0)
//and LFO Waveform (SINE, SAW, INVERTED_SAW, SQUARE and PULSE). 
//I amd now using the two Control Change (CC) Banks as following:
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
//point calculation, but that was not the case, it got worse, and I referted
//to floating point. Therefore, the Oscillator produces samples using frloating point.
//Introduced a new waveshape oprion XTREME in addition to the existing ones. XTREME
//will implement all the features of paraphony (for the moment it just mixes SAW and
//SQUARE_PULSE.
//----------------------------------------------------------------------------------
//12 Mar 2015: I start here the final version of my syntheziser based on Teensy 3.1 
//microcontroller, now names as SPinSynth-01. This version starts fromm everything 
//I have developed so far as Synth-01-Teensy and it is already a fully operational 
//syntheziser with one VA Oscillator, VA Filter, VA Amplifier, Envelope Generator
//and MIDI interface. Now I will start introducing additional features.
//----------------------------------------------------------------------------------

#ifndef SynthUtilities_h
#define SynthUtilities_h
#include "SynthUtilities.h"
#endif

#include "Oscillator.h"
// #include "XtOscillator.h"
#include "Filter.h"
#include  "Amplifier.h"
#include <Arduino.h>

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#ifndef SynthUtilities_h
#define SynthUtilities_h
#include "SynthUtilities.h"
#endif

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
 
 float sampleRate = 44100.0; // Sample Rate = 44.1KHz
 const int waveTableSize = 2048;
 float maxAmplitude = 4097.0;
 float halfMaxAmplitude = maxAmplitude * 0.4;
 float amplitudeDCO1 = 1.0;
 float deg;
 float rad;
 int sineWave;
 long startTime;
 long duration;
 
 int outputBufferSize = 128;
 
 DoubleBuffer oscillatorBuffer;
 DoubleBuffer filterBuffer;
 DoubleBuffer amplifierBuffer;
 
 ModWheelFunction mModWheenFunction = LFO_OSCILLATOR;
 
 int updatePhase = 0;
 
 float* oscillatorBuffer0;
 float* oscillatorBuffer1;
 float* filterBuffer0;
 float* filterBuffer1;
 float* outputBuffer;  //Buffer to be read by the DAC output.
 float* writeBuffer;   //Buffer to be written by the Oscillator.getNextBuffer.
 boolean getNextOscillatorBuffer = FALSE;
 boolean synchDAC = FALSE;
 int outputSample = 0;
 
 float minFrequency = 22.0; //minimum frequency is 40Hz
 float maxFrequency = 2200.0; //maximum frequency is 6KHz
 float frequency = minFrequency; // start at minFrequency
 
 float frequencyIncrement = 1.0;
 float frequencyCount = 0.0;
 
 float sampleIncrement;
 float sampleIncrementFactor = float(waveTableSize) / sampleRate;
 unsigned long kp = waveTableSize * 4096;
 
 //This is new for Teensy
 
 IntervalTimer updatevaOscillatorTimer;
 IntervalTimer updateDCO;
 IntervalTimer updateControls;

 //---------------- MIDI Callbacks -------------------------------
 void HandleNoteOn(byte channel, byte pitch, byte velocity) { 
  // int newPitch;
  /*
  Serial.print("MIDI NoteON ");
  Serial.print(" Channel ");
  Serial.print(channel);
  Serial.print(" Note ");
  Serial.print(pitch);
  Serial.print(" Velocity ");
  Serial.println(velocity);
  */
  
  vaOscillator.noteON(int(pitch), int(velocity));
  vaFilter.trigger();
  vaFilter.noteON(int(pitch), int(velocity));
  vaAmplifier.trigger();
  vaAmplifier.noteON(int(pitch), int(velocity));
}

//RP: I created this Callback
void HandleNoteOff(byte channel, byte pitch, byte velocity){
  // int newPitch = -1;
  /*
  Serial.print("MIDI NoteOFF ...");
  Serial.print(" Channel ");
  Serial.print(channel);
  Serial.print(" Note ");
  Serial.print(pitch);
  Serial.print(" Velocity ");
  Serial.println(velocity);
  */
  vaOscillator.noteOFF(int(pitch), int(velocity));
  vaFilter.noteOFF(int(pitch), int(velocity));
  vaAmplifier.noteOFF(int(pitch), int(velocity));
}

//RP: I created this Callback - ModWheel is a ControlChange
//ControlChange sends the numeric values of potentiometers and these are associated with
//a Control like Filter Frequency and Resonance, and Envelope Parameters (ADSR).
void HandleControlChange(byte channel, byte number, byte value) { 
  float mValue, pulseW;
  char* waveshape = "";
  //int intValue;
  //intValue = int(value);
/*
  Serial.print("MIDI ControlChange ");
  Serial.print(" Channel ");
  Serial.print(channel);
  Serial.print(" Control #");
  Serial.print(number);
  Serial.print(" Value ");
  Serial.println(value);
*/
  switch (number){
    case 1: //Modulation Wheel is controlling LFO Amount for Oscillator frequency
      switch (mModWheenFunction){
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
    case 41: //Master Tuning
      vaOscillator.setTuning(value);
      break;
    case 18: // Determines the function of the Pitch Bend Whel
      if(value <= 20){
        //Filter Cutoff
       mModWheenFunction = CUTOFF;
      }
      else if((value > 21) && (value <= 42)){
        //Filter Resonance
        mModWheenFunction = RESONANCE;
      }
      else if((value > 42) && (value <= 62)){
        //LFO Filter
        mModWheenFunction = LFO_FILTER;
      }
      else if((value > 62) && (value <= 118)){
        //LFO Oscillator
         mModWheenFunction = LFO_OSCILLATOR;
      }
      else if(value > 120){
        //LFO Oscillator
        mModWheenFunction = LFO_OSCILLATOR;
      }
      break;
    case 16:
      vaOscillator.setLFOrate(value);
      break;
    case 17:
      vaOscillator.setLFOamount(value);
      break;
    case 91:
      waveshape = "";
      if(value <= 16){
        vaOscillator.setLFOwaveform(LFO_SINE);
      }
      else if((value > 16) && (value <= 32)){
        vaOscillator.setLFOwaveform(LFO_SAW);
      }
      else if((value > 32) && (value <= 48)){
        vaOscillator.setLFOwaveform(LFO_INVERTED_SAW);
      }
      else if((value > 48) && (value <= 64)){
        vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
        vaOscillator.setLFOpulsewidth(0.5);
      }
      else if(value > 64){
        mValue = float(value - 64) / 64.0;
        vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
        vaOscillator.setLFOpulsewidth(0.5 * (1.0 - 0.85 * mValue));
      }
    case 42:
      mValue = float(value) / 127.0;
      vaOscillator.setSawAmount(mValue);
      break;
     case 37:
      mValue = 100.0 * (-1.0 + float(value) / 63.5);
      vaOscillator.setSawXfactor(mValue);
      break;
    case 43:
      mValue = float(value) / 127.0;
      vaOscillator.setPulseAmount(mValue);
      break;
    case 38:
      pulseW = 0.5 - float(value) / 317.0;
      vaOscillator.setPulseWidth(pulseW);
      break;
    case 44:
      mValue = float(value) / 127.0;
      vaOscillator.setSubAmout(mValue);
      break;
    case 39:
      mValue = 0.5 + float(value) / 127.0;
      vaOscillator.setSubFactor(mValue);
      break;
    case 74:
      vaFilter.setDialCutoff(value);
      break;
    case 71:
      vaFilter.setDialResonance(value);
      break;
    case 76:
      vaFilter.setLFOrate(value);
      break;
    case 77:
      vaFilter.setLFOamount(value);
      break;
    case 93:
      waveshape = "";
      if(value <= 16){
        vaFilter.setLFOwaveform(LFO_SINE);
      }
      else if((value > 16) && (value <= 32)){
        vaFilter.setLFOwaveform(LFO_SAW);
      }
      else if((value > 32) && (value <= 48)){
        vaFilter.setLFOwaveform(LFO_INVERTED_SAW);
      }
      else if((value > 48) && (value <= 64)){
        vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
        vaFilter.setLFOpulsewidth(0.5);
      }
      else if(value > 64){
        mValue = float(value - 64) / 64.0;
        vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
        vaFilter.setLFOpulsewidth(0.5 * (1.0 - 0.85 * mValue));
      }
    case 73:
      vaFilter.setAttack(value);
      break;
    case 75:
      vaFilter.setDecay(value);
      break;
    case 79:
      vaFilter.setSustain(value);
      break;
    case 72:
      vaFilter.setRelease(value);
      break;
    case 19:
      vaFilter.setEnvelopeLevel(value);
      break;
    case 80:
      vaAmplifier.setAttack(value);
      break;
    case 81:
      vaAmplifier.setDecay(value);
      break;
    case 82:
      vaAmplifier.setSustain(value);
      break;
    case 83:
      vaAmplifier.setRelease(value);
      break;
    case 7:
      vaAmplifier.setVolume(value);
      break;
    case 40:
      waveshape = "";
      if(value <= 20){
        waveshape = "SINE";
        vaOscillator.setWaveform(SINE);
      }
      else if((value > 21) && (value <= 42)){
        waveshape = "SAW";
        vaOscillator.setWaveform(SAW);
      }
      else if((value > 42) && (value <= 62)){
        waveshape = "SQUARE_PULSE";
        vaOscillator.setWaveform(SQUARE_PULSE);
        vaOscillator.setPulseWidth(0.5);
      }
      else if((value > 62) && (value <= 118)){
        waveshape = "SQUARE_PULSE";
        mValue = float(value - 62) / 62.0;
        vaOscillator.setWaveform(SQUARE_PULSE);
        vaOscillator.setPulseWidth(0.5 * (1.0 - 0.85 * mValue));
      }
      else if(value > 120){
        waveshape = "XTREME";
        vaOscillator.setWaveform(XTREME);
        vaOscillator.setPulseWidth(0.5);
      }
      /*
      Serial.print("Control Change #");
      Serial.print(number);
      Serial.print(" Channel ");
      Serial.print(channel);
      Serial.print(" : Waveform ");
      Serial.print(waveshape);
      Serial.print(" ");
      Serial.println(value);
      */
      break;
    case 35:
      vaOscillator.setPortamento(int(value));
      /*
      Serial.print("Control Change #");
      Serial.print(number);
      Serial.print(" Channel ");
      Serial.print(channel);
      Serial.print(" : Portamento ");
      Serial.print(waveshape);
      Serial.print(" ");
      Serial.println(value);
      */
      break;  
    default:
      /*
      Serial.print("Control Change #");
      Serial.print(number);
      Serial.print(" Channel ");
      Serial.print(channel);
      Serial.print(" : UNASSIGNED ");
      Serial.println(value);
      */
      break;
    //break;
  }
  /*
  Serial.print("MIDI ControlChange ");
  Serial.print(" Channel ");
  Serial.print(channel);
  Serial.print(" Control #");
  Serial.print(number);
  Serial.print(" Value ");
  Serial.println(value);
  */
}

//RP: I created this Callback
void HandlePitchBend(byte channel, int bend) { 
  vaOscillator.setPitchBend(bend);
  /*
  Serial.print("MIDI PitchBend ");
  Serial.print(" Channel ");
  Serial.print(channel);
  Serial.print(" Bend ");
  Serial.println(bend);
  */
  //vaOscillator.setFrequency(mFrequency);
}
  
//----------------- End MIDI Callbacks -------------------------

//startBuffer() initiallises the "write" and "output" buffers.
 void startOscillatorBuffer(){
   outputSample = 0;
   writeBuffer = oscillatorBuffer0;
   outputBuffer = oscillatorBuffer1;
   getNextOscillatorBuffer = TRUE;
 }
 
 //swapBuffer() swaps the "write" and "output" buffers.
 void swapOscillatorBuffer(){
   float* tempBuffer;
   outputSample = 0;
   tempBuffer = outputBuffer;
   outputBuffer = writeBuffer;
   writeBuffer = tempBuffer;
   getNextOscillatorBuffer = TRUE;
 }
 
 // outputBufferedDAC() sends datat from the outputBuffer to the DAC.
 //The outputBuffer is filled by the Oscillator with outputBufferSize samples
 //in one call to vaOscillator.getNextBuffer(buffer, bufferSize).
 void outputBufferedDAC(){
   // float oscillatorSample;
   // float filterSample;
   // long lFilterSample;
   long lAmplifierSample;
   int dacOutput;
   // float* readBuffer;
   /*
   outputSample++;
   if(outputSample == (outputBufferSize)){
     swapOscillatorBuffer();
   }
   oscillatorSample = outputBuffer[outputSample];
   */
  
   //readBuffer = oscillatorBuffer.getReadBuffer();
   //oscillatorSample = readBuffer[outputSample];
   //oscillatorSample = outputBuffer[outputSample];
   
   outputSample++;
   if(outputSample == (outputBufferSize)){
     oscillatorBuffer.swapBuffer();
     filterBuffer.swapBuffer();
     outputSample = 0;
     synchDAC = TRUE;
   }
  
   //oscillatorSample = oscillatorBuffer.read();
  
   //filterSample = vaFilter.update(oscillatorSample);
   
   //lFilterSample = vaFilter.lUpdate(long(FixedPoint::convertToFP(oscillatorSample)));
   ///dacOutput = int(halfMaxAmplitude * (1.25 + float(lFilterSample) / 65536.0));
   
   //lFilterSample = filterBuffer.read();
   //dacOutput = 2048 + ((lFilterSample >> 5));
   
   lAmplifierSample = amplifierBuffer.read();
   dacOutput = 2048 + ((lAmplifierSample >> 5));
   
   
   //filterSample = filterBuffer.read();
   //lFilterSample = filterSample * floatFactor;
   
   //dacOutput = 2048 + ((lFilterSample >> 5));
   
   //filterSample = oscillatorSample;
   //dacOutput = int(halfMaxAmplitude * (1.25 + filterSample));
   //dacOutput = int(halfMaxAmplitude * (1.0 + outputBuffer[outputSample] * amplitudeDCO1));
   analogWrite(A14, dacOutput);
 }
 
 //updateSingleDAC() is the interrupt callback used to update one single sample
 //obtained directly from the Oscillator.
 void updateSingleDAC(){
   float nextSampleOscillator;
   float nextSampleFilter;
   float nextSample;
   int dacOutput;
   nextSampleOscillator = vaOscillator.getNaiveSample();
   nextSampleFilter = vaFilter.update(nextSampleOscillator);
   nextSample = nextSampleFilter;
   dacOutput = int(halfMaxAmplitude * (1.0 + nextSample));
   analogWrite(A14, dacOutput);
 }

void printDuration()
{
  duration = micros() - startTime;
  Serial.print("Duration = ");
  Serial.print(duration);
  Serial.println(" us");
}

//amplitudeSweep() is a Test Envelope Generator that creates a Decay of 1 second.
 
 void amplitudeController(){
  amplitudeDCO1 = amplitudeDCO1 - 0.001;
  if(amplitudeDCO1 <= 0.0){
    amplitudeDCO1 = 1.0;
  }
  //Test amplitude
  amplitudeDCO1 = 1.0;
}

//frequencySweep() sets the frequency of the Oscillator from min to max.

void frequencySweep(){
  float k;
  k = pow(2.0, frequencyCount / 10000.0);
  frequency = minFrequency * k;
  if(frequency > maxFrequency){
    frequency = minFrequency;
    frequencyCount = 0;
    //Serial.println("****");
  }
  //Test Frequency
  //frequency = 6000.0;
 
  frequencyCount = frequencyCount + frequencyIncrement; 
  vaOscillator.setFrequency(frequency);                                           
}

void update(){
  ///*
  if(filterBuffer.isReadyToWrite() && updatePhase == 1){
    vaFilter.updateBuffer(oscillatorBuffer, filterBuffer);
    //vaFilter.updateBufferFloat(oscillatorBuffer, filterBuffer);
    vaAmplifier.updateBuffer(filterBuffer, amplifierBuffer);
    updatePhase = 0;
    //Serial.println("F");
  }
  if(oscillatorBuffer.isReadyToWrite() && synchDAC){
    vaOscillator.updateBuffer(oscillatorBuffer);
    //Option to remove Anti-Alysiasing correction with Poly-Blep
    //vaOscillator.updateNaiveBuffer(oscillatorBuffer);
    updatePhase = 1;
    synchDAC = FALSE;
    //Serial.println("O");
  }
  //*/
  vaOscillator.update();
  vaFilter.update();
  vaAmplifier.update();
}

void controlUpdate() {
  update();
  //frequencySweep();
  //amplitudeController();
}

void setup() {
  startTime = millis();
  Serial.begin(9600);
  analogWriteResolution(12);
  
  outputBufferSize = 128;
  synchDAC = FALSE;
  getNextOscillatorBuffer = FALSE;
  updatePhase = 0;
  
  pinMode(LED, OUTPUT);
  pinMode(TUNED, OUTPUT);
  pinMode(FLAT, OUTPUT);
  pinMode(SHARP, OUTPUT);
  
  oscillatorBuffer0 = new float[outputBufferSize];
  oscillatorBuffer1 = new float[outputBufferSize];
  
  startOscillatorBuffer();
  
  oscillatorBuffer.start(outputBufferSize);
  filterBuffer.start(outputBufferSize);
  amplifierBuffer.start(outputBufferSize);
 
  //Initialise Teensy's IntervalTimers
  
  //Single sample output to DAC.
  //updatevaOscillatorTimer.begin(updateSingleDAC, 1000000.0 / sampleRate); //Parameters are: Function to be called and interval is in µseconds
 
  //Buffered output to DAC: update is called every 22.6 µseconds (44.1KHz rate)
  updatevaOscillatorTimer.begin(outputBufferedDAC, 1000000.0 / sampleRate); //Parameters are: Function to be called and interval is in µseconds
  updatevaOscillatorTimer.priority(10);
  updateControls.begin(controlUpdate, 250.0); //Controls are updated at 250.0µseconds or 0.25mseconds
  updateControls.priority(128);
  ///*
  //MIDI Setup
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);    
  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  MIDI.setHandleControlChange(HandleControlChange);  // Put only the name of the function
  MIDI.setHandlePitchBend(HandlePitchBend);  // Put only the name of the function 
  
  usbMIDI.begin();    
  usbMIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  usbMIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  usbMIDI.setHandleControlChange(HandleControlChange);  // Put only the name of the function
  usbMIDI.setHandlePitchChange(HandlePitchBend);  // Put only the name of the function 
}

void loop(){

  if(MIDI.read()){
     // Serial.println("MIDI Read");
   }
   
  if (usbMIDI.read()) {
    /*
    Serial.print("USB type=");
    Serial.print(usbMIDI.getType());
    Serial.print(" ch=");
    Serial.print(usbMIDI.getChannel());
    Serial.print(" d1=");
    Serial.print(usbMIDI.getData1());
    Serial.print(" d2=");
    Serial.println(usbMIDI.getData2());
    */
  }

  //Use the code below to measure the processing time to generate
  //the oscillator data for one buffer. 
  /*
   //vaOscillator.setWaveform(SAW);
   Serial.print("SAW wave generation f= ");
   Serial.print(frequency); Serial.print(" ");
   startTime = micros();
   vaOscillator.getNextBuffer(writeBuffer, outputBufferSize);
   printDuration();
   delay(1000);
  */
   /*
   vaOscillator.setFrequency(440.0);
   vaOscillator.setWaveform(SINE);
   Serial.print("SINE wave generation ");
   startTime = micros();
   vaOscillator.updateBuffer(oscillatorBuffer);
   printDuration();
   delay(100);
   vaOscillator.setWaveform(SAW);
   Serial.print("SAW wave generation ");
   startTime = micros();
   vaOscillator.updateBuffer(oscillatorBuffer);
   printDuration();
   delay(100);
   vaOscillator.setWaveform(SQUARE_PULSE);
   Serial.print("SQUARE wave generation ");
   startTime = micros();
   vaOscillator.updateBuffer(oscillatorBuffer);
   printDuration();
   delay(100);
   vaOscillator.setWaveform(XTREME);
   Serial.print("XTREME wave generation ");
   startTime = micros();
   vaOscillator.updateBuffer(oscillatorBuffer);
   printDuration();
   delay(100);
   */
  
  /*
   Serial.print("Oscillator Buffer update ");
   vaOscillator.setFrequency(1760.0);
   vaOscillator.setWaveform(SAW);
   startTime = micros();
   vaOscillator.updateNaiveBuffer(oscillatorBuffer);
   printDuration();
   delay(100);
    Serial.print("Filter Buffer update ");
   startTime = micros();
   vaFilter.updateBufferFloat(oscillatorBuffer, filterBuffer);
   printDuration();
   delay(100);
  */
 }
