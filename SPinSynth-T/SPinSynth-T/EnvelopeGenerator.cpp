//---------------------------------------------------
//Amplifier.cpp - Last update: 19 Mar 2015 - Started: 25 Feb 2015.
//---------------------------------------------------
//19 Mar 2015: Moved the Note ON LED to the EnvelopeGenerator Class.
//---------------------------------------------------
//09 Mar 2015: Calibrated the constants to define offset Xo and coefficiet Cf
//as suggested by Will Pirkle's book. The Envelope Generator is working fine,
//although further adjustments may be needed.
//---------------------------------------------------
//06 Mar 2015: Implemented updateBuffer using Double Buffer mechanism.
//updateBuffer calculates the fileter using Fixed Point Integer numbers.
//---------------------------------------------------
//05 Mar 2015: Implemented the VA Filter with Fixed Point numbers as long integer.
//See lUpdate(long input) and the constants in SynthUtilities.h.
//---------------------------------------------------
//Implementes a Virtual Analog (VA) Filter with the following features:
//24DB per octave; Cuttof and Resonance control and ADSR Envelope
//controlling the Cuttof frequency.
//This Filter is based on Martin Finke's Filter: Making Audio Plugins Part 13
//-----------------------------------------------------------

#include <Arduino.h>
#include "EnvelopeGenerator.h"
#include <math.h>

void EnvelopeGenerator::start(){
 setAttack(0);
 setDecay(0);
 setDecay(127);
 setRelease(0);
}

float EnvelopeGenerator::exp(float x){
  return pow(eBase, x);
}

void EnvelopeGenerator::setUpdateRate(float updateRate){
  mUpdateRate = updateRate;
}

void EnvelopeGenerator::setTrigger(){
  mTrigger = ON;
  //Serial.println("Envelope Trigger ON");
}

void EnvelopeGenerator::setGateON(){
  mOnCount++;
  if(mOnCount > 0){
    mGate = ON;
   digitalWrite(LED, HIGH);
  }
  //Serial.println("Envelope Note ON");
}

void EnvelopeGenerator::setGateOFF(){
  mOnCount--;
  if(mOnCount == 0){
    mGate = OFF;
   digitalWrite(LED, LOW);
  }
  //Serial.println("Envelope Note OFF");
}

void EnvelopeGenerator::setAttack(int attack){
  mAttack = float(attack + 1) * 10.0;
  mAttackCf = exp(ECOA / mAttack);
  mAttackXo = (1.0 + TCOA) * (1.0 - mAttackCf);
  /*
  Serial.print("Attack ");
  Serial.print(mAttack);
  Serial.print(" Xo= ");
  Serial.print(mAttackXo);
  Serial.print(" Cf= ");
  Serial.println(mAttackCf);
  */
}

void EnvelopeGenerator::setDecay(int decay){
  mDecay = float(decay) * 100.0 +1.0;
  mDecayCf = exp(ECODR / mDecay);
  mDecayXo = (mSustain - TCODR) * (1.0 - mDecayCf);
  /*
  Serial.print("Decay ");
  Serial.print(mDecay);
  Serial.print(" Xo= ");
  Serial.print(mDecayXo);
  Serial.print(" Cf= ");
  Serial.println(mDecayCf);
  */
}

void EnvelopeGenerator::setSustain(int sustain){
  mSustain = float(sustain) / 127.0;
  mDecayXo = (mSustain - TCODR) * (1.0 - mDecayCf);
  /*
  Serial.print("Sustain ");
  Serial.println(mSustain);
  */
}

void EnvelopeGenerator::setRelease(int release){
  mRelease = float(release + 1) * 100.0;
  mReleaseCf = exp(ECODR / mRelease);
  mReleaseXo = -TCODR * (1.0 - mReleaseCf);
  /*
  Serial.print("Release ");
  Serial.print(mRelease);
  Serial.print(" Xo= ");
  Serial.print(mReleaseXo);
  Serial.print(" Cf= ");
  Serial.println(mReleaseCf);
  */
}

void EnvelopeGenerator::setVolume(int volume){
  mVolume = float(volume) / 127.0;
  lVolume = FixedPoint::convertToFP(mVolume);
}

float EnvelopeGenerator::update(){
  if(mTrigger){
    mState = ATTACK;
    mGate = ON;
    mTrigger = OFF;
  }
  if(mGate == OFF){
    mState = RELEASE;
  }
  switch(mState){
    case _OFF:{
      mOutput = 0.0;
    }
    case ATTACK:{
      mOutput = mAttackXo + mOutput * mAttackCf;
      if(mOutput >= 1.0){
        mOutput = 1.0;
        mState = DECAY;
      }
      break; 
    }
    case DECAY:{
      mOutput = mDecayXo + mOutput * mDecayCf;
      if(mGate <= mSustain){
        mOutput = mSustain;
        mState = SUSTAIN;
      }
      break; 
    }
    case SUSTAIN:{
      mOutput = mSustain;
      break; 
    }
    case RELEASE:{
      mOutput = mReleaseXo + mOutput * mReleaseCf;
      if(mOutput <= 0.0){
        mOutput = 0.0;
        mState = _OFF;
      }
      break; 
    }
  }
  /*
  if(mGate == ON){
    mOutput = 1.0;
  }
  else{
    mOutput = 0.0;
  }
  */
  //Serial.print("Env output = ");
  //Serial.println(mOutput);
  return mOutput;
}

