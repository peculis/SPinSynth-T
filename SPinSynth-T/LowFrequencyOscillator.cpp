//-----------------------------------------------------------------------
//LowFrequencyOscillator.cpp - Last update: 14 Mar 2015 - Started: 14 Feb 2015.
//-----------------------------------------------------------------------
//14 Mar 2015: LowFrequencyOscillator (LFO) is a simplified oscillator based on
//the Oscillator class meant to be used as controller and modulator.
//The following parameters can be controlled:
//LFO Rate (0.1 to 12.7Hz), amount (0.0 to 1.0) and
//LFO Waveform (SINE, SAW, INVERTED_SAW, SQUARE and PULSE). 
//-----------------------------------------------------------------------

#include "LowFrequencyOscillator.h"

const float kx = 1.2; //Defines the waveshape amplitude -/+ 1.0.

void LowFrequencyOscillator::update(){
  //Do nothing for now.
}

float* LowFrequencyOscillator::generateSineWaveTable(int size) {
  float* sineWaveTable = new float[size];
  float k = mPI / (size / 2.0); // factor to convert degree to radian.
  int sample;
  for (int deg = 0 ; deg < size; deg++)
  {
    sample = int(deg);
    sineWaveTable[sample] = kx * sin(deg * k);
  } 
  return sineWaveTable;
}

float* LowFrequencyOscillator::generateSawWaveTable(int size) {
  float* sawWaveTable = new float[size];
  for (int n = 0 ; n < size; n++)
  {
    sawWaveTable[n] = kx * (2.0 * float(n) / float(size) -1.0);
  } 
  return sawWaveTable;
}

void LowFrequencyOscillator::setWaveform(LFOwaveform waveform) {
  mWaveform = waveform;
}

LFOwaveform LowFrequencyOscillator::getWaveform() {
  return (mWaveform);
}

void LowFrequencyOscillator::setFrequency(float frequency) {
  mFrequency = frequency;
  updateIncrement(frequency);
}

float LowFrequencyOscillator::getFrequency(){
  return mFrequency;
}

void LowFrequencyOscillator::setAmplitude(float amplitude) {
  mAmplitude = amplitude;
}

float LowFrequencyOscillator::getAmplitude() {
  return mAmplitude;
}

void LowFrequencyOscillator::setPulseWidth(float pulseWidth) {
  mPulseWidth = pulseWidth;
  mPulseThreshold = pulseWidth * mWaveTableSize;
  lPulseThreshold = long(mPulseThreshold * 65536.0);
  mPulseOffset = 2.0 * (0.5 - mPulseWidth);
}

void LowFrequencyOscillator::setSampleRate(float sampleRate) {
  mSampleRate = sampleRate;
  mIncrementFactor = mWaveTableSize / mSampleRate;
}

void LowFrequencyOscillator::updateIncrement(float frequency) {
  mPhaseIncrement = frequency * mIncrementFactor;
  lPhaseIncrement = long(mPhaseIncrement * 65536.0);
}

float LowFrequencyOscillator::getSample() {
  float sample = 0.0;
  lPhase += lPhaseIncrement;
    if(lPhase >= lWaveTableSize){
      lPhase = long(lPhase - lWaveTableSize);
    }
  switch (mWaveform) {
    case LFO_SINE:
      sample = mSineWaveTable[int(lPhase >> 16)];
      break;
    case LFO_SAW:;
      sample = mSawWaveTable[int(lPhase >> 16)];
      break;
    case LFO_INVERTED_SAW:;
      sample = 0.0 - mSawWaveTable[int(lPhase >> 16)];
      break;
    case LFO_SQUARE_PULSE:
     if(lPhase >= lPulseThreshold){
          sample = -kx;
     }
     else{
          sample = kx;
     }
     break;
  }
  return (mAmplitude * sample);
}
