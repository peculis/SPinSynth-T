//-----------------------------------------------------------------------
//LowFrequencyOscillator.h - Last update: 14 Mar 2015 - Started: 14 Feb 2015.
//-----------------------------------------------------------------------
//14 Mar 2015: LowFrequencyOscillator (LFO) is a simplified oscillator based on
//the Oscillator class meant to be used as controller and modullator.
//The following parameters can be controlled:
//LFO Rate (0.1 to 12.7Hz), amout (0.0 to 1.0) and
//LFO Waveform (SINE, SAW, INVERTED_SAW, SQUARE and PULSE). 
//-----------------------------------------------------------------------

///*
#ifndef SynthUtilities_h
#define SynthUtilities_h
#include "SynthUtilities.h"
#endif
//*/

#include <math.h>

enum LFOwaveform {
   LFO_SINE,
   LFO_SAW,
   LFO_INVERTED_SAW,
   LFO_SQUARE_PULSE
 };
  
class LowFrequencyOscillator{
  
//const int mWaveTableSize = 2048;
//float* mSineWaveTable = (generateSineWaveTable(2048));
  
private:
  LFOwaveform mWaveform;
  const float mPI;
  const float twoPI = 2.0 * mPI;
  
  float mFrequency = 0.0;
  float mAmplitude = 0.0;
  float mPulseWidth = 0.5;
  float mPulseOffset = 0;
  float mPulseThreshold;
  float mSquareThreshold;
  
  float mPhase;
  float mPhaseIncrement;
  
  //Pitch Ben
  int mPitchBend;
  
  long lPhase;
  long lPhaseIncrement;
  long lWaveTableSize;
  long lHalfWaveTableSize;
  long lPulseThreshold;
  long lSquareThreshold; 
  
  float mLastSample;
  float mNextSample;
  
  float mSampleRate;
  const int mWaveTableSize = 256;
  
  const long lMaxPhase = long(mWaveTableSize * 65536);
  const float fMaxPhase = float(lMaxPhase);
  
  const float* mSineWaveTable = generateSineWaveTable(mWaveTableSize);
  const float* mSawWaveTable = generateSawWaveTable(mWaveTableSize);
  
  float mIncrementFactor = mWaveTableSize / mSampleRate;
  
  void  updateIncrement(float frequency);
  float* generateSineWaveTable(int size);
  float* generateSawWaveTable(int size);

public:
  void setWaveform(LFOwaveform waveform);
  LFOwaveform getWaveform();
  void update();
  void setFrequency(float frequency);
  float getFrequency();
  void setAmplitude(float amplitude);
  float getAmplitude();
  void setPulseWidth(float pulseWidth);
  void setSampleRate(float sampleRate);
  float getSample();
  
  LowFrequencyOscillator():
    mWaveform(LFO_SINE),
    mPI(2*acos(0.0)),
    mFrequency(1.0),
    mWaveTableSize (256) ,
    mSampleRate(2000.0),
    mPulseWidth(0.50),
    lPhase(0),
    mPhase(0.0) { 
      mPulseThreshold = mPulseWidth * mWaveTableSize;
      mPulseOffset = 2.0 * (0.5 - mPulseWidth);
      mSquareThreshold = 0.5 * mWaveTableSize;
      lWaveTableSize = long(mWaveTableSize << 16);
      lPulseThreshold = long(mPulseThreshold * 65536.0);
      lSquareThreshold = long(mSquareThreshold * 65536.0);
      updateIncrement(mFrequency); };
};

