//-----------------------------------------------------------------------
//Oscillator.h - Last update: 3 Apr 2015 - Started: 12 Feb 2015.
//-----------------------------------------------------------------------
//3 Apr 2015: Created SynthTables that contains a number of tables used for
//Waveform Generation, Anti-Aliasing BLEP correction and MIDI Pitch to Frequency.
//Implemented Master Tuning +/- OneTone - Bank2 P7, 63 = 440Hz.
//-----------------------------------------------------------------------
//2 Apr 2015: Implemented anti-aliasing Blep algorithm as a table.
//-----------------------------------------------------------------------
//29 Mar 2015: Implemented Note ON/OFF event management to replicate the behaviour of
//monophonic keyboards where when two notes are presses simjultaneously the higher note
//will play and when the highest key is released the heigher key immediately below
//that is still pressed will play.
//-----------------------------------------------------------------------
//16 Mar 2015: Changed XTREME waveform to implement Detuned and SubOscillator
//Detuned detunes the a second SAW wavefor by +/- 100 Cents (or one semi-tone)
//SubOscillator adds a third SAW waveform with frequency one octave below (P3 Bank 2)
//or one fifth above (P5 Bank2). PULSE WIDTH is controlled by P4 Bank 2. The amount 
//of XTREME SAW, PULSE and SUB can be controlled separatelly by P8, P9 and P10 Bank 2.
//-----------------------------------------------------------------------
//15 Mar 2015: Implemented XTREME SAW modulated by two LFOs. Still requires tuning,
//and additional control, but it works.
//-----------------------------------------------------------------------
//13 Mar 2015: Resolved the "glitch" caused by high processing load. Initially 
//I thought I had improved performance by replacing floating point with fixed
//point calculation, but that was not the case, it got worse, and I referted
//to floating point. Therefore, the Oscillator produces samples using frloating point.
//Introduced a new waveshape oprion XTREME in addition to the existing ones. XTREME
//will implement all the features of paraphony (for the moment it just mixes SAW and
//SQUARE_PULSE.
//-----------------------------------------------------------------------
//12 Mar 2015: Oscillator or Extreme Oscillator extands the original Oscillator 
//class with paraphony capabilities: SAW -> XSAW, SQUARE_PULSE (as before) and
//an Overtone Oscillator.
//-----------------------------------------------------------------------

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "SynthUtilities.h"

#include <math.h>
#include "SynthTables.h"
//#include "midi_pitch_table.h"

#include "LowFrequencyOscillator.h"

#define FLAT 14
#define TUNED 15
#define SHARP 16

enum Waveform {
   SINE,
   SAW,
   SQUARE_PULSE,
   XTREME
 };
  
class Oscillator{
  
//const int mWaveTableSize = 2048;
//float* mSineWaveTable = (generateSineWaveTable(2048));
  
private:
  Waveform mWaveform;
  const float mPI;
  const float twoPI = 2.0 * mPI;
  const float oneCent = pow(2.0, 1.0 / 1200.0);
  const float twoCent = pow(oneCent, 2.0);
  const float oneSemiTone = pow(2.0, 1.0 / 12.0) - 1.0;
  const float oneTone = pow(2.0, 1.0 / 6.0) - 1.0;
  
  //Store Key Events.
  int maxKeyEvents = 32;
  KeyEvent* keyEvents = new KeyEvent [maxKeyEvents];  
  
  LowFrequencyOscillator LFO1;
  
  float mPulseWidth = 0.3;
  float mPulseOffset = 0;
  long  lPulseOffset =FixedPoint::convertToFP(mPulseOffset);
  float mPulseThreshold;
  float mSquareThreshold;
  long lWaveTableSize;
  long lHalfWaveTableSize;
  long lPulseThreshold;
  long lSquareThreshold; 
  
  float mPulseAmount = 0.0;
  long lPulseAmount = FixedPoint::convertToFP(mPulseAmount);
  float mSawAmount = 1.0;
  long lSawAmount = FixedPoint::convertToFP(mSawAmount);
  float mSawXfactor = 0.0;
  long lSawXfactor = FixedPoint::convertToFP(mSawXfactor);
  float mSubAmount = 1.0;
  long lSubAmount = FixedPoint::convertToFP(mSubAmount);
  float mSubFactor = 1.0;
  
  
  //Main Frequency, Phase and Increment
  float mFrequency;
  float mTargetFrequency;
  float mPhase;
  float mPhaseIncrement;
  long lPhase;
  long lPhaseIncrement;
  
  //Offtune Frequency, Phase and Increment
  float mOffFrequency;
  float mOffPhase;
  float mOffPhaseIncrement;
  long lOffPhase;
  long lOffPhaseIncrement;
  
  //SubOscillator Frequency, Phase and Increment
  float mSubFrequency;
  float mSubPhase;
  float mSubPhaseIncrement;
  long lSubPhase;
  long lSubPhaseIncrement;
  
  //Portamento
  int mPortamento = 0;
  float mPortamentoTime = 0.0;
  float mPortamentoIncrements = 0.0;
  float mPortamentoUpdateRate = 0.001;
  float mEndFrequency;
  float mStartFrequency;
  float mPortamentoFactor = 1.0;
  
  //Master Tuning -63 to +64
  int mTuning = 0;
  
  //Pitch Bend
  int mPitchBend;
  
  float mLastSample;
  float mNextSample;
  
  float mSampleRate;
  //const int mWaveTableSize = 256;
  
  const long lMaxPhase = long(mWaveTableSize * 65536);
  const float fMaxPhase = float(lMaxPhase);
  
  float mIncrementFactor = mWaveTableSize / mSampleRate;
  
  void  updateIncrement(float frequency);
  float* generateSineWaveTable(int size);
  float* generateSawWaveTable(int size);
  // float* generateBlepRisingTable(int size);
  // float* generateBlepFallingTable(int size);
  float getSawSample(long phase, long inc);
  //long getSawSample(long phase, long inc);
  void updatePortamento();
  float updateTuning(float frequency);
  float updatePitchBend(float frequency);
  float updateLFO(float frequency);
  
  void allocateKeyEvent(int pitch, int state, int velocity);
  int getPitch();

public:
  void setWaveform(Waveform waveform);
  Waveform getWaveform();
  void update();
  void noteON(int pitch, int velocity);
  void noteOFF(int pitch, int velocity);
  void setFrequency(float frequency);
  float getFrequency();
  void setTuning(int pitch);
  void setPitch(int pitch);
  void setPitchBend(int bend);
  void setPortamento(int portamento);
  void setPulseAmount(float amount);
  void setPulseWidth(float pulseWidth);
  void setSawAmount(float amount);
  void setSawXfactor(float xFactor);
  void setSubAmout(float amount);
  void setSubFactor(float factor);
  void setSampleRate(float sampleRate);
  void setLFOrate(int rate);
  void setLFOamount(int anount);
  void setLFOwaveform(LFOwaveform waveform);
  void setLFOpulsewidth(float pulsewidth);
  float getNaiveSample();
  float getSample();
  void updateBuffer(DoubleBuffer buffer);
  void updateNaiveBuffer(DoubleBuffer buffer);
  
  Oscillator():
    mWaveform(SQUARE_PULSE),
    mPI(2*acos(0.0)),
    mPulseWidth(0.50),
    mFrequency(440.0),
    mPhase(0.0),
    lPhase(0),
    mSampleRate(44100.0) { 
      mStartFrequency = mFrequency;
      mEndFrequency = mFrequency;
      mTargetFrequency = mFrequency;
      mPulseThreshold = mPulseWidth * mWaveTableSize;
      mPulseOffset = 2.0 * (0.5 - mPulseWidth);
      mSquareThreshold = 0.5 * mWaveTableSize;
      lWaveTableSize = long(mWaveTableSize << 16);
      lPulseThreshold = long(mPulseThreshold * 65536.0);
      lSquareThreshold = long(mSquareThreshold * 65536.0);
      updateIncrement(mFrequency); };
};

#endif
