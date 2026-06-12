//-------------------------------------------------------------------------------------
//Filter.h - Last update: 4 Apr 2015 - Started: 25 Feb 2015.
//-------------------------------------------------------------------------------------
//4 Apr 2015: Changed the default Cutoff value to 0.9 (filter open).
//-------------------------------------------------------------------------------------
//2 Apr 2015: Reduced the range of Cutoff (see calculateCutoff()) to keep the filter stable.
//-------------------------------------------------------------------------------------
//29 Mar 2015: Implemented LFO and Mod Wheel Cutoff Modulation and improved Cutoff 
//Modulation in general. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Cutoff Modulation,
//-------------------------------------------------------------------------------------
//13 Mar 2015: Reverted the filter input to floating point (see the XtOscillator class.
//-------------------------------------------------------------------------------------
//10 Mar 2015: Implemented Envelope Control to Filter including
//Envelope Level and inversion.
//---------------------------------------------------
//06 Mar 2015: Implemented updateBuffer using Double Buffer mechanism.
//updateBuffer calculates the filter using Fixed Point Integer numbers.
//---------------------------------------------------
//05 Mar 2015: Implemented the VA Filter with Fixed Point numbers as long integer.
//See updateBuffer() and the constants in SynthUtilities.h.
//---------------------------------------------------
//Implements a Virtual Analog (VA) Filter with the following features:
//24DB per octave; Cutoff and Resonance control and ADSR Envelope
//controlling the Cutoff frequency. 
//This Filter is based on Martin Finke's Filter: Making Audio Plugins Part 13
//-----------------------------------------------------------

#ifndef FILTER_H
#define FILTER_H

#include "SynthUtilities.h"

#include "EnvelopeGenerator.h"

#include "LowFrequencyOscillator.h"

class Filter{
public:
  // blockTime = bufferSize / sampleRate = 128 / 44100 = 2.9msec
  // transitionTime = 30msec
  // smoothFactor - alpha = blockTime / transitionTime = 0.097
  float blockTime = 128.0f / 44100.0f;
  float transitionTime = 0.050f;
  float alpha = blockTime / transitionTime; // about 0.097
  float update(float input);
  void update();
  void updateBuffer(DoubleBuffer bufferIn, DoubleBuffer bufferOut);
  void setDialCutoff(int cutoff);
  void setModCutoff(int cutoff);
  void setDialResonance(int resonance);
  void setAttack(int attack);
  void setDecay(int decay);
  void setSustain(int sustain);
  void setRelease(int release);
  void setEnvelopeLevel(int level);
  void setLFOrate(int rate);
  void setLFOamount(int amount);
  void setLFOwaveform(LFOwaveform waveform);
  void setLFOpulsewidth(float pulsewidth);
  void trigger();
  void noteON(int pitch, int velocity);
  void noteOFF(int pitch, int velocity);
  Filter():
    mCutoff(0.9),
    mResonance(0.0),
    s0(0.0),
    s1(0.0),
    s2(0.0),
    s3(0.0){
      calculateFeedback();
  };
   
private:
  int mPitch;
  float mVelocity;
  long lCutoff;
  long lFeedback;
  long ls0;
  long ls1;
  long ls2;
  long ls3;
  int   DialCutoff;
  int   ModCutoff;
  int   Cutoff;
  float mCutoff = 0.9;
  float mDialCutoff = 0.9;
  float mDialResonance = 0.1;
  float mModCutoff = 0.0;
  float mEnvelopeCutoff;
  float mLFOCutoff;
  float mResonance = 0.5;
  float mFeedback;
  LowFrequencyOscillator LFO1;
  float updateLFO(float cutoff);
  float updateLFO();
  EnvelopeGenerator envelope;
  const float kmEnvLevel = 0.5;
  float mEnvDynamics;
  float mEnvelopeLevel = 0.0;
  float s0;
  float s1;
  float s2;
  float s3;
  void calculateFeedback();
  void calculateCutoff();
};

#endif
