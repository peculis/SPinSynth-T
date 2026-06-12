//---------------------------------------------------
//EnvelopeGenerator.h - Last update: 19 Mar 2015 - Started: 07 Mar 2015.
//---------------------------------------------------
//19 Mar 2015: Moved the Note ON LED to the EnvelopeGenerator Class.
//---------------------------------------------------
//09 Mar 2015: Calibrated the constants to define offset Xo and coefficient Cf
//as suggested by Will Pirkle's book. The Envelope Generator is working fine,
//although further adjustments may be needed.
//---------------------------------------------------

#ifndef ENVELOPE_GENERATOR_H
#define ENVELOPE_GENERATOR_H

#include <Arduino.h>

#include "SynthUtilities.h"

enum EnvelopeState {
   _OFF,
   ATTACK,
   DECAY,
   SUSTAIN,
   RELEASE
 };

class EnvelopeGenerator{
public:
   void start();
   void setTrigger();
   void setGateON();
   void setGateOFF();
   float update();
   void setAttack(int attack);
   void setDecay(int decay);
   void setSustain(int sustain);
   void setRelease(int release);
   void setVolume(int volume);
   void setUpdateRate(float rate); //Update rate in miliseconds
   
private:
  const float eBase = 2.71828183; //number "e" for "exponetial" operation.
  const float TCOA = 0.99999;  //Time Constant Overshoot for Attack
  const float ECOA = -0.6932;  //Exponential Constant Overshoot for Attack
  const float TCODR = 0.00001; //Time Constant Overshoot for Decay and Release
  const float ECODR = -11.5129; //Exponential Constant Overshoot for Decay and Release
  float mUpdateRate = 0.001; //Update rate in miliseconds
  EnvelopeState mState = _OFF;
  int mTrigger = OFF;
  int mGate = OFF;
  int mOnCount = 0;
  float mOutput = 0.0;
  float mAttack = 1.0;
  float mAttackXo = (1.0 + TCOA) * (1.0 - mAttackCf);
  float mAttackCf = exp(ECOA / mAttack);
  float mDecay = 100.0;
  float mDecayXo = (mSustain - TCODR) * (1.0 - mDecayCf);
  float mDecayCf = exp(ECODR / mDecay);
  float mSustain = 1.0;
  float mRelease = 100.0;
  float mReleaseXo = -TCODR * (1.0 - mReleaseCf);
  float mReleaseCf = exp(ECODR / mRelease);
  float mVolume = 1.0;
  long  lVolume = long(floatFactor);
  long  lOutputLevel = 0;
  float exp(float x); //returns e**x or pow(e, x);
};

#endif
