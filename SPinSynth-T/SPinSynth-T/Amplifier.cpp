//------------------------------------------------------------------------------
//Amplifier.cpp - Last update: 4 Apr 2015 - Started: 25 Feb 2015.
//------------------------------------------------------------------------------
//4 Apr 2015: Implemented master Volume CC#7.
//------------------------------------------------------------------------------
//29 Mar 2015: Implemented also the effect of Key Velocity applied to the
//amount of Envelope Amplitude Modulation.
//------------------------------------------------------------------------------
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

#include "Amplifier.h"

void Amplifier::setAttack(int attack){
  //mAttack = attack / 127.0;
  // Serial.println("Amp ATTACK");
  envelope.setAttack(attack);
}

void Amplifier::setDecay(int decay){
  //mDecay = decay / 127.0;
  // Serial.println("Amp DECAY");
  envelope.setDecay(decay);
}

void Amplifier::setSustain(int sustain){
  //mSustain = sustain / 127.0;
  // Serial.println("Amp sustain");
  envelope.setSustain(sustain);
}

void Amplifier::setRelease(int release){
  //mRelease = release / 127.0;
  // Serial.println("Amp RELEASE");
  envelope.setRelease(release);
}

void Amplifier::setVolume(int volume){
  mVolume = volume / 127.0;
  lVolume = FixedPoint::convertToFP(mVolume);
}

float Amplifier::update(float input){
  return input;
}

long Amplifier::lUpdate(long input){
  return input;
}

void Amplifier::update(){
  envelopeLevel = FixedPoint::convertToFP(envelope.update());
}

void Amplifier::trigger(){
  // Serial.println("Amplifier Trigger");
  envelope.setTrigger();
}

void Amplifier::noteON(int pitch, int velocity){
  // Serial.println("Amplifier Note ON");
  mPitch = pitch;
  mVelocity = float(velocity) / 127.0;
  lVelocity = FixedPoint::convertToFP(mVelocity + 0.1);
  envelope.setGateON();
}

void Amplifier::noteOFF(int pitch, int velocity){
  // Serial.println("Amplifier Note OFF");
  envelope.setGateOFF();
}

void Amplifier::updateBufferFloat(DoubleBuffer bufferIn, DoubleBuffer bufferOut){
  float input;
   while(bufferOut.isReadyToWrite()){
    input = bufferIn.read();
    bufferOut.write(input);
  }
 }

void Amplifier::updateBuffer(DoubleBuffer bufferIn, DoubleBuffer bufferOut){
  long input, level;
  level = FixedPoint::multiply(envelopeLevel, lVelocity);
  level = FixedPoint::multiply(level, lVolume);
  while(bufferOut.isReadyToWrite()){
    input = bufferIn.read();
    //bufferOut.write(FixedPoint::multiply(input, envelopeLevel));
    bufferOut.write(FixedPoint::multiply(input, 2.0 * level));
  }
}
