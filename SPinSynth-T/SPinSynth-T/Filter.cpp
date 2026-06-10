//-------------------------------------------------------------------------------------
//Filter.cpp - Last update: 2 Apr 2015 - Started: 25 Feb 2015.
//-------------------------------------------------------------------------------------
//2 Apr 2015: Reduced the range of Cutoff (see calculateCutoff()) to prevevt the Filter 
//to become unstable.
//-------------------------------------------------------------------------------------
//29 Mar 2015: Implemented LFO and Mod Wheel Cutoff Modulation and improved Cutoff 
//Modulation in general. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Cutoff Modulation,
//-------------------------------------------------------------------------------------
//13 Mar 2015: Reverted the filter input to floaing point (see the XtOscillator class.
//---------------------------------------------------
//10 Mar 2015: Implemented Envelope Control to Filter including
//Envelope Level and inversion.
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

#include "Filter.h"

void Filter::calculateFeedback(){
  float xCutoff;
  float newResonance;
  //lCutoff = FixedPoint::convertToFP(mCutoff);
  //lEnvelopeCutoff = lCutoff;
  //mEnvelopeCutoff = mCutoff;
  //lResonance = FixedPoint::convertToFP(mResonance);
  //xCutoff = 0.9;
  newResonance = mResonance + alpha * (mDialResonance - mResonance);
  if(newResonance >= 0.95){
    newResonance = 0.95; //Reduced the range to prevevt the Filter to become unstable.
  }
  else if(newResonance <= 0.01){
    newResonance = 0.01; //Reduced the range to prevevt the Filter to become unstable.
  }
  mResonance = newResonance;
  mFeedback = double((mResonance + mResonance / (1.0 - (mCutoff))));
  //mFeedback = double((mDialResonance + mDialResonance / (1.0 - (mCutoff))));
  lFeedback = FixedPoint::convertToFP(mFeedback);
  lCutoff = FixedPoint::convertToFP(mCutoff);
  //lEnvelopeCutoff = lCutoff;
  //lResonance = FixedPoint::convertToFP(mResonance);
  //Serial.print("calculateResonance ");
  //Serial.println(mResonance);
}


void Filter::calculateCutoff(){
  float newCutoff;
  long tempEnvelopeLevel;
  //tempEnvelopeLevel = FixedPoint::multiply(lEnvelopeLevel, klEnvLevel);
  //lEnvelopeCutoff = lCutoff +  FixedPoint::multiply(tempEnvelopeLevel, envelopeLevel);
  //newCutoff = mDialCutoff + mModCutoff + mLFOCutoff + mEnvelopeCutoff * mEnvelopeLevel;
  mCutoff += alpha * (mDialCutoff - mCutoff);
  newCutoff = mCutoff + mModCutoff + mLFOCutoff + 0.1 * mEnvelopeCutoff * mEnvDynamics;
  if(newCutoff >= 0.99){
    newCutoff = 0.99; //Reduced the range to prevevt the Filter to become unstable.
  }
  else if(newCutoff <= 0.01){
    newCutoff = 0.01; //Reduced the range to prevevt the Filter to become unstable.
  }
   mCutoff = newCutoff;
  //Serial.print("calculateCutoff ");
  //Serial.println(mCutoff);
}

void Filter::setMode(FilterMode mode){
  mMode = mode;
}

void Filter::setDialCutoff(int cutoff){
  DialCutoff = cutoff;
  Cutoff = DialCutoff;
  mDialCutoff = float(Cutoff) / 127.0; 
  //mCutoff = mDialCutoff;
  Serial.print("DialCutoff ");
  Serial.println(cutoff);
  //calculateSmoothCutoff();
  //calculateSmoothFeedback();
  calculateCutoff();
  calculateFeedback();
}

void Filter::setModCutoff(int cutoff){
  ModCutoff = cutoff;
  mModCutoff = (float(ModCutoff) / 127.0);
  mCutoff = mModCutoff;
  //Serial.println(cutoff);
  //calculateSmoothCutoff();
  //calculateSmoothFeedback();
  calculateCutoff();
  calculateFeedback();
}

void Filter::setDialResonance(int resonance){
  mDialResonance = float(resonance) / 127.0;
  if( mDialResonance >= 0.95){
    mDialResonance = 0.95; //Reduced the range to prevevt the Filter to become unstable.
  }
  else if(mDialResonance <= 0.01){
    mDialResonance = 0.01; //Reduced the range to prevevt the Filter to become unstable.
  }
  //Serial.print("DialResonance ");
  //Serial.println(resonance);
  //calculateSmoothFeedback();
  calculateFeedback();
}

void Filter::setAttack(int attack){
  //mAttack = attack / 127.0;
  envelope.setAttack(attack);
}

void Filter::setDecay(int decay){
  //mDecay = decay / 127.0;
  envelope.setDecay(decay);
}

void Filter::setSustain(int sustain){
  //mSustain = sustain / 127.0;
  envelope.setSustain(sustain);
}

void Filter::setRelease(int release){
  //mRelease = release / 127.0;
  envelope.setRelease(release);
}

void Filter::setEnvelopeLevel(int level){
  if(level <= 63){
    mEnvelopeLevel = kmEnvLevel * ((float(level) / 63.0) - 1.0);
  }
  else{
    mEnvelopeLevel = kmEnvLevel * (1.0 - float(127 - level) / 63.0);
  }
  //mEnvDynamics = mEnvelopeLevel * (0.5 + mVelocity);
  //lEnvelopeLevel = FixedPoint::convertToFP(mEnvelopeLevel);
  //Serial.print("Filter Envelope Level = ");
  //Serial.println(mEnvelopeLevel);
}

void Filter::trigger(){
  //Serial.print("Filter ");
  envelope.setTrigger();
}

void Filter::noteON(int pitch, int velocity){
  //Serial.print("Filter ");
  mPitch = pitch;
  mVelocity = float(velocity) / 127.0;
  //lVelocity = FixedPoint::convertToFP(mVelocity);
  mEnvDynamics = mEnvelopeLevel * (0.5 + mVelocity);
  //Serial.print("Filter Envelope Dynamics = ");
  //Serial.println(mEnvDynamics);
  envelope.setGateON();
}

void Filter::noteOFF(int pitch, int velocity){
  //Serial.print("Filter ");
  envelope.setGateOFF();
}

void Filter::update(){
  //envelopeLevel = FixedPoint::convertToFP(envelope.update());
  //Serial.println("Filter update");
  mLFOCutoff = LFO1.getSample();
  mEnvelopeCutoff = envelope.update();
  calculateCutoff();
  calculateFeedback();
}

void Filter::setLFOrate(int rate){
  float LFOrate;
  LFOrate = float(rate) / 10.0;
  //Serial.print("Filter ");
  LFO1.setFrequency(LFOrate);
  //Serial.print("LFO Rate = "); 
  //Serial.println(LFOrate);
}

void Filter::setLFOamount(int amount){
  float LFOamount;
  LFOamount = float(amount) / 511.0;
  //Serial.print("Filter ");
  LFO1.setAmplitude(LFOamount);
  //Serial.print("LFO Amount = "); 
  //Serial.println(LFOamount);
}

void Filter::setLFOwaveform(LFOwaveform waveform){
  float LFOamount;
  //Serial.print("Filter ");
  LFO1.setWaveform(waveform);
  //Serial.print("LFO Waveform = "); 
  //Serial.println(waveform);
}

void Filter::setLFOpulsewidth(float pulsewidth){
  LFO1.setPulseWidth(pulsewidth);
  //Serial.print("LFO Pulsewidth = "); 
  //Serial.println(pulsewidth);
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

long Filter::lUpdate(long input){
   ls0 += FixedPoint::multiply(lEnvelopeCutoff, ((input - ls0) + FixedPoint::multiply(lFeedback, (ls0 - ls1))));
   ls1 += FixedPoint::multiply(lEnvelopeCutoff, (ls0 - ls1));
   ls2 += FixedPoint::multiply(lEnvelopeCutoff, (ls1 - ls2));
   ls3 += FixedPoint::multiply(lEnvelopeCutoff, (ls2 - ls3));
  return ls0;
}

void Filter::updateBufferFloat(DoubleBuffer bufferIn, DoubleBuffer bufferOut){
  float input;
  while(bufferOut.isReadyToWrite()){
    input =bufferIn.read();
    s0 += mCutoff * (input - s0 + mFeedback * (s0 - s1));
    s1 += mCutoff * (s0 - s1);
    s2 += mCutoff * (s1 - s2);
    s3 += mCutoff * (s2 - s3);
    bufferOut.write(s0);
  }
 }

void Filter::updateBuffer(DoubleBuffer bufferIn, DoubleBuffer bufferOut){
  long input;
  while(bufferOut.isReadyToWrite()){
    input = FixedPoint::convertToFP(bufferIn.read());
    //input = bufferIn.read();
    ls0 += FixedPoint::multiply(lCutoff, ((input - ls0) + FixedPoint::multiply(lFeedback, (ls0 - ls1))));
    ls1 += FixedPoint::multiply(lCutoff, (ls0 - ls1));
    ls2 += FixedPoint::multiply(lCutoff, (ls1 - ls2));
    ls3 += FixedPoint::multiply(lCutoff, (ls2 - ls3));
    bufferOut.write(ls0);
  }
}
