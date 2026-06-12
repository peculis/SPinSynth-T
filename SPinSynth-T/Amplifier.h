//---------------------------------------------------
//Amplifier.h - Last update: 4 Apr 2015 - Started: 07 Mar 2015.
//------------------------------------------------------------------------------
//4 Apr 2015: Implemented master Volume CC#7.
//------------------------------------------------------------------------------
//29 Mar 2015: Implemented also the effect of Key Velocity applied to the
//amount of Envelope Amplitude Modulation.
//------------------------------------------------------------------------------

#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include "SynthUtilities.h"

#include "EnvelopeGenerator.h"

class Amplifier{
public:
   void update();
   void updateBuffer(DoubleBuffer bufferIn, DoubleBuffer bufferOut);
   void setAttack(int attack);
   void setDecay(int decay);
   void setSustain(int sustain);
   void setRelease(int release);
   void setVolume(int volume);
   void trigger();
   void noteON(int pitch, int velocity);
   void noteOFF(int pitch, int velocity);
   
private:
  EnvelopeGenerator envelope;
  long envelopeLevel;
  float mVolume = 1.0;
  long  lVolume = long(floatFactor);
  int mPitch;
  float mVelocity;
  long  lVelocity;
};

#endif
