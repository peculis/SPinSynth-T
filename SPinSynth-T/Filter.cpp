//-------------------------------------------------------------------------------------
//Filter.cpp - Last update: 2 Apr 2015 - Started: 25 Feb 2015.
//-------------------------------------------------------------------------------------
//2 Apr 2015: Reduced the range of Cutoff (see calculateCutoff()) to keep the filter stable.
//-------------------------------------------------------------------------------------
//29 Mar 2015: Implemented LFO and Mod Wheel Cutoff Modulation and improved Cutoff 
//Modulation in general. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Cutoff Modulation,
//-------------------------------------------------------------------------------------
//13 Mar 2015: Reverted the filter input to floating point (see the XtOscillator class.
//---------------------------------------------------
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

#include "Filter.h"

void Filter::calculateFeedback(){
  float newResonance;
  newResonance = mResonance + alpha * (mDialResonance - mResonance);
  if(newResonance >= 0.95){
    newResonance = 0.95; //Clamp the range to keep the filter stable.
  }
  else if(newResonance <= 0.01){
    newResonance = 0.01; //Clamp the range to keep the filter stable.
  }
  mResonance = newResonance;
  mFeedback = double((mResonance + mResonance / (1.0 - (mCutoff))));
  lFeedback = FixedPoint::convertToFP(mFeedback);
  lCutoff = FixedPoint::convertToFP(mCutoff);
}


void Filter::calculateCutoff(){
  float newCutoff;
  mCutoff += alpha * (mDialCutoff - mCutoff);
  newCutoff = mCutoff + mModCutoff + mLFOCutoff + 0.1 * mEnvelopeCutoff * mEnvDynamics;
  if(newCutoff >= 0.99){
    newCutoff = 0.99; //Clamp the range to keep the filter stable.
  }
  else if(newCutoff <= 0.01){
    newCutoff = 0.01; //Clamp the range to keep the filter stable.
  }
  mCutoff = newCutoff;
}

void Filter::setDialCutoff(int cutoff){
  DialCutoff = cutoff;
  Cutoff = DialCutoff;
  mDialCutoff = float(Cutoff) / 127.0; 
  calculateCutoff();
  calculateFeedback();
}

void Filter::setModCutoff(int cutoff){
  ModCutoff = cutoff;
  mModCutoff = (float(ModCutoff) / 127.0);
  mCutoff = mModCutoff;
  calculateCutoff();
  calculateFeedback();
}

void Filter::setDialResonance(int resonance){
  mDialResonance = float(resonance) / 127.0;
  if(mDialResonance >= 0.95){
    mDialResonance = 0.95; //Clamp the range to keep the filter stable.
  }
  else if(mDialResonance <= 0.01){
    mDialResonance = 0.01; //Clamp the range to keep the filter stable.
  }
  calculateFeedback();
}

void Filter::setAttack(int attack){
  envelope.setAttack(attack);
}

void Filter::setDecay(int decay){
  envelope.setDecay(decay);
}

void Filter::setSustain(int sustain){
  envelope.setSustain(sustain);
}

void Filter::setRelease(int release){
  envelope.setRelease(release);
}

void Filter::setEnvelopeLevel(int level){
  if(level <= 63){
    mEnvelopeLevel = kmEnvLevel * ((float(level) / 63.0) - 1.0);
  }
  else{
    mEnvelopeLevel = kmEnvLevel * (1.0 - float(127 - level) / 63.0);
  }
}

void Filter::trigger(){
  envelope.setTrigger();
}

void Filter::noteON(int pitch, int velocity){
  mPitch = pitch;
  mVelocity = float(velocity) / 127.0;
  mEnvDynamics = mEnvelopeLevel * (0.5 + mVelocity);
  envelope.setGateON();
}

void Filter::noteOFF(int pitch, int velocity){
  envelope.setGateOFF();
}

void Filter::update(){
  mLFOCutoff = LFO1.getSample();
  mEnvelopeCutoff = envelope.update();
  calculateCutoff();
  calculateFeedback();
}

void Filter::setLFOrate(int rate){
  float LFOrate;
  LFOrate = float(rate) / 10.0;
  LFO1.setFrequency(LFOrate);
}

void Filter::setLFOamount(int amount){
  float LFOamount;
  LFOamount = float(amount) / 511.0;
  LFO1.setAmplitude(LFOamount);
}

void Filter::setLFOwaveform(LFOwaveform waveform){
  LFO1.setWaveform(waveform);
}

void Filter::setLFOpulsewidth(float pulsewidth){
  LFO1.setPulseWidth(pulsewidth);
}

float Filter::updateLFO(){
  return LFO1.getSample();
}

float Filter::updateLFO(float cutoff){
  float lfo;
  lfo = LFO1.getSample();

  if(lfo > 0.0){
    cutoff = cutoff * (1.0 + lfo);
  }
  else if(lfo < 0.0){
    cutoff = cutoff * (1.0 + 0.5 * lfo);
  }
  return cutoff;
}

float Filter::update(float input){
  s0 += mCutoff * (input - s0 + mFeedback * (s0 - s1));
  s1 += mCutoff * (s0 - s1);
  s2 += mCutoff * (s1 - s2);
  s3 += mCutoff * (s2 - s3);
  return s0;
}

void Filter::updateBuffer(DoubleBuffer bufferIn, DoubleBuffer bufferOut){
  long input;
  while(bufferOut.isReadyToWrite()){
    input = FixedPoint::convertToFP(bufferIn.read());
    ls0 += FixedPoint::multiply(lCutoff, ((input - ls0) + FixedPoint::multiply(lFeedback, (ls0 - ls1))));
    ls1 += FixedPoint::multiply(lCutoff, (ls0 - ls1));
    ls2 += FixedPoint::multiply(lCutoff, (ls1 - ls2));
    ls3 += FixedPoint::multiply(lCutoff, (ls2 - ls3));
    bufferOut.write(ls0);
  }
}
