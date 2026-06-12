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
  envelope.setAttack(attack);
}

void Amplifier::setDecay(int decay){
  envelope.setDecay(decay);
}

void Amplifier::setSustain(int sustain){
  envelope.setSustain(sustain);
}

void Amplifier::setRelease(int release){
  envelope.setRelease(release);
}

void Amplifier::setVolume(int volume){
  mVolume = volume / 127.0;
  lVolume = FixedPoint::convertToFP(mVolume);
}

void Amplifier::update(){
  envelopeLevel = FixedPoint::convertToFP(envelope.update());
}

void Amplifier::trigger(){
  envelope.setTrigger();
}

void Amplifier::noteON(int pitch, int velocity){
  mPitch = pitch;
  mVelocity = float(velocity) / 127.0;
  lVelocity = FixedPoint::convertToFP(mVelocity + 0.1);
  envelope.setGateON();
}

void Amplifier::noteOFF(int pitch, int velocity){
  envelope.setGateOFF();
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
