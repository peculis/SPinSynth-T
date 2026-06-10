//-----------------------------------------------------------------------
//Oscillator.cpp - Last update: 10 Apr 2015 - Started: 12 Feb 2015.
//-----------------------------------------------------------------------
//10 Apr: Implemened Tuning LEDs.
//-----------------------------------------------------------------------
//3 Apr 2015: Created SynthTables that contains a number of tables used for
//Waveform Generation, Anti-Aliasing BLEP correction and MIDI Pitch to Frequency.
//Implemented Master Tuning +/- OneTone - Bank2 P7, 63 = 440Hz.
//-----------------------------------------------------------------------
//2 Apr 2015: Implemented anti-aliasing Blep algorithm as a table.
//-----------------------------------------------------------------------
//29 Mar 2015: Implemented Note ON/OFF event management to replicate the behaviour of
//monophonic keyboards where when two notes are presses simjultaneously the higher note
//will play and when the highest key is released the heigher key immediately below
//that is still pressed will play.
//-----------------------------------------------------------------------
//16 Mar 2015: Changed XTREME waveform to implement Detuned and SubOscillator
//Detuned detunes the a second SAW wavefor by +/- 100 Cents (or one semi-tone)
//SubOscillator adds a third SAW waveform with frequency one octave below (P3 Bank 2)
//or one fifth above (P5 Bank2). PULSE WIDTH is controlled by P4 Bank 2. The amount 
//of XTREME SAW, PULSE and SUB can be controlled separatelly by P8, P9 and P10 Bank 2.
//-----------------------------------------------------------------------
//15 Mar 2015: Implemented XTREME SAW modulated by two LFOs. Still requires tuning,
//and additional control, but it works.
//-----------------------------------------------------------------------
//13 Mar 2015: Resolved the "glitch" caused by high processing load. Initially 
//I thought I had improved performance by replacing floating point with fixed
//point calculation, but that was not the case, it got worse, and I referted
//to floating point. Therefore, the Oscillator produces samples using frloating point.
//Introduced a new waveshape oprion XTREME in addition to the existing ones. XTREME
//will implement all the features of paraphony (for the moment it just mixes SAW and
//SQUARE_PULSE.
//-----------------------------------------------------------------------
//12 Mar 2015: Oscillator or Extreme Oscillator extands the original Oscillator 
//class with paraphony capabilities: SAW -> XSAW, SQUARE_PULSE (as before) and
//an Overtone Oscillator.
//-----------------------------------------------------------------------

#include <Arduino.h>
#include "Oscillator.h"

//const float kx = 0.7; //Defines the waveshape amplitude -/+ 1.0.

void Oscillator::update(){
  float frequency;
  updatePortamento();
  mFrequency = mTargetFrequency;
  frequency = updateTuning(mFrequency);
  frequency = updatePitchBend(frequency);
  frequency = updateLFO(frequency);
  //Limit maximum frequency.
  //max frequency 4190 Hz. I was limitting on 2093 Hz
  if(frequency > 4190.0){
    frequency = 4190.0;
  }
/*
  if(frequency > 2093.0){
    frequency = 2093.0;
  }
*/
//Serial.println(frequency);
updateIncrement(frequency);
}


void Oscillator::allocateKeyEvent(int pitch, int state, int velocity){
  int newPitch;
  //Serial.println("allocateKeyEvent");
   if(state == ON){
     //Serial.println("allocateKeyEvent   ON");
     for(int k = 0; k < maxKeyEvents; k++){
       if(keyEvents[k].state == OFF){
         if(keyEvents[k].age < maxKeyEvents){
           keyEvents[k].age++;
           //Serial.println("-------");
         }
       }
     }
     for(int k = 0; k < maxKeyEvents; k++){
       if(keyEvents[k].state == OFF){
         if(keyEvents[k].age > 3){
           keyEvents[k].age = 0;
           keyEvents[k].pitch = pitch;
           keyEvents[k].state = ON;
           keyEvents[k].velocity = velocity;
           /*
           Serial.print("Allocate Note ON k = ");
           Serial.print(k);
           Serial.print(" age = ");
           Serial.print(keyEvents[k].age);
           Serial.print(" pitch = ");
           Serial.println(keyEvents[k].pitch);
           */
           k = maxKeyEvents;
         }
       }
     }
   }
   else if(state == OFF){
     //Serial.println("allocateKeyEvent   OFF");
     for(int k = 0; k < maxKeyEvents; k++){
       if(keyEvents[k].pitch == pitch){
         if(keyEvents[k].state == ON){
           keyEvents[k].state = OFF;
           keyEvents[k].velocity = velocity;
           /*
           Serial.print("Allocate Note OFF k = ");
           Serial.print(k);
           Serial.print(" age = ");
           Serial.print(keyEvents[k].age);
           Serial.print(" pitch = ");
           Serial.println(keyEvents[k].pitch);
           */
           //k = maxKeyEvents;
         }
       }
     }
   }
}

int Oscillator::getPitch(){
   int newPitch = -1;
   for(int k = 0; k < maxKeyEvents; k++){
     if(keyEvents[k].state == ON){
       //Serial.println("++++");
       if(keyEvents[k].pitch > newPitch){
         newPitch = keyEvents[k].pitch;
       }
     }
   }
   //Serial.print("NEW PITCH = ");
   //Serial.println(newPitch);
   return newPitch;
}

void Oscillator::noteON(int pitch, int velocity){
  allocateKeyEvent(pitch, ON, velocity);
  setPitch(getPitch());
}

void Oscillator::noteOFF(int pitch, int velocity){
  int newPitch;
  allocateKeyEvent(pitch, OFF, velocity);
  newPitch = getPitch();
   if(newPitch > 0){
    setPitch(newPitch);
  }
  else{
    setPitch(pitch);
  }
}

float* Oscillator::generateSineWaveTable(int size) {
  float* sineWaveTable = new float[size];
  float k = mPI / (size / 2.0); // factor to convert degree to radian.
  int sample;
  for (int deg = 0 ; deg < size; deg++)
  {
    sample = int(deg);
    sineWaveTable[sample] = kx * sin(deg * k);
    //sineWaveTable[sample] = FixedPoint::convertToFP(kx * sin(deg * k));
  } 
  return sineWaveTable;
}

float* Oscillator::generateSawWaveTable(int size) {
  float* sawWaveTable = new float[size];
  for (int n = 0 ; n < size; n++)
  {
    sawWaveTable[n] = kx * ((2.0 * n / size) -1.0);
    //sawWaveTable[n] = FixedPoint::convertToFP(kx * (2.0 * float(n) / float(size) -1.0));
  } 
  return sawWaveTable;
}

void Oscillator::setWaveform(Waveform waveform) {
  mWaveform = waveform;
}

Waveform Oscillator::getWaveform() {
  return (mWaveform);
}

void Oscillator::setPulseAmount(float amount) {
  mPulseAmount = amount * 0.6;
  //lPulseAmount =FixedPoint::convertToFP(mPulseAmount);
}

void Oscillator::setSawAmount(float amount) {
  mSawAmount = amount * 0.6;
  //lSawAmount =FixedPoint::convertToFP(mSawAmount);
}

void Oscillator::setSawXfactor(float factor) {
  if(factor >= 0.0){
    mSawXfactor = pow(oneCent, factor);
  }
  else{
    factor = (-1.0 * factor);
    mSawXfactor = 1.0 / pow(oneCent, factor);
  }
  //lSawXfactor =FixedPoint::convertToFP(mSawXfactor);
}

void Oscillator::setSubAmout(float amount) {
  mSubAmount = amount * 0.6;
  //lSubAmount =FixedPoint::convertToFP(mSubAmount);
}

void Oscillator::setSubFactor(float factor) {
  mSubFactor = factor;
  //lSubfactor =FixedPoint::convertToFP(mSubFactor);
}

void Oscillator::setFrequency(float frequency) {
  mFrequency = frequency;
  //updateIncrement();
}

float Oscillator::getFrequency(){
  return mFrequency;
}

void Oscillator::setPitch(int pitch){
  mStartFrequency = mFrequency;
  mEndFrequency = midiPitchTable[pitch];
  if(mPortamento == 0){
    mTargetFrequency = mEndFrequency;
    mStartFrequency = mFrequency;
    mPortamentoTime = 0.0;
    mPortamentoIncrements = 0;
    mPortamentoFactor = 1.0;
  }
  else{
    mPortamentoFactor = pow(mEndFrequency / mStartFrequency, 1.0 / mPortamentoIncrements);
  }
  //updateIncrement();
}

void Oscillator::setTuning(int pitch){
  mTuning = int(pitch - 63);
  //Serial.print("Master Tuning = "); 
  //Serial.println(mTuning);
}

void Oscillator::setPitchBend(int bend){
  mPitchBend = int(bend);
}

void Oscillator::setLFOrate(int rate){
  float LFOrate;
  LFOrate = float(rate) / 10.0;
  LFO1.setFrequency(LFOrate);
  //Serial.print("LFO Rate = "); 
  //Serial.println(LFOrate);
}

void Oscillator::setLFOamount(int amount){
  float LFOamount;
  LFOamount = float(amount) / 127.0;
  LFO1.setAmplitude(LFOamount);
  //Serial.print("LFO Amount = "); 
  //Serial.println(LFOamount);
}

void Oscillator::setLFOwaveform(LFOwaveform waveform){
  float LFOamount;
  LFO1.setWaveform(waveform);
  //Serial.print("LFO Waveform = "); 
  //Serial.println(waveform);
}

void Oscillator::setLFOpulsewidth(float pulsewidth){
  LFO1.setPulseWidth(pulsewidth);
  //Serial.print("LFO Pulsewidth = "); 
  //Serial.println(pulsewidth);
}

float Oscillator::updateLFO(float frequency){
  float lfo, sawPhase2, sawPhase3;
  lfo = LFO1.getSample();
 
  if(lfo > 0.0){
      frequency = frequency * (1.0 + lfo);
  }
  else if(lfo < 0.0){
      frequency = frequency * (1.0 + 0.5 * lfo);
      }
  return frequency;
}

float Oscillator::updateTuning(float frequency){
  if(mTuning > 0){
      frequency = frequency * (1.0 + oneTone * float(mTuning) / 64.0);
       digitalWrite(TUNED, LOW);
       digitalWrite(FLAT, LOW);
       digitalWrite(SHARP, HIGH);
  }
  else if(mTuning < 0){
      frequency = frequency * (1.0 + oneTone * (float(mTuning) / 63.0));
       digitalWrite(TUNED, LOW);
       digitalWrite(FLAT, HIGH);
       digitalWrite(SHARP, LOW);
  }
  else if(mTuning == 0){
    digitalWrite(TUNED, HIGH);
    digitalWrite(FLAT, LOW);
    digitalWrite(SHARP, LOW);
  }
  return frequency;
}

float Oscillator::updatePitchBend(float frequency){
  if(mPitchBend > 0){
      frequency = frequency * (1.0 + float(mPitchBend) / 8191.0);
  }
  else if(mPitchBend < 0){
      frequency = frequency * (1.0 + 0.5 * (float(mPitchBend) / 8192.0));
      }
  return frequency;
}

void Oscillator::setPortamento(int portamento){
  mPortamento = portamento;
  if(mPortamento == 0){
    //mFrequency = mEndFrequency;
    //mStartFrequency = mPitchBend;
    mPortamentoTime = 0.0;
    mPortamentoIncrements = 1.0;
    mPortamentoFactor = 1.0;
  }
  else{
    mPortamentoTime = 0.002 * float(mPortamento);
    mPortamentoIncrements = mPortamentoTime / mPortamentoUpdateRate;
  }
}

void Oscillator::updatePortamento(){
  if(mPortamento == 0){
    //mFrequency = mEndFrequency;
    //mStartFrequency = mPitchBend;
    mPortamentoTime = 0.0;
    mPortamentoIncrements = 0;
    mPortamentoFactor = 1.0;
  }
  else{
    mTargetFrequency = mTargetFrequency * mPortamentoFactor;
    if(mEndFrequency > mStartFrequency){
      if(mTargetFrequency >= mEndFrequency){
        mTargetFrequency = mEndFrequency;
      }
    }
    else if(mEndFrequency < mStartFrequency){
      if(mTargetFrequency <= mEndFrequency){
         mTargetFrequency = mEndFrequency;
      }
    }
  }
  //updateIncrement();
}

void Oscillator::setPulseWidth(float pulseWidth) {
  mPulseWidth = pulseWidth;
  mPulseThreshold = pulseWidth * mWaveTableSize;
  lPulseThreshold = long(mPulseThreshold * 65536.0);
  mPulseOffset = 2.0 * (0.5 - mPulseWidth);
  lPulseOffset = FixedPoint::convertToFP(mPulseOffset);
  //mPulseOffset = 1.2 * (0.5 - mPulseWidth); //Used when k = 0.6, i.e. waveshape amplitude = -/+ 0.6
}

void Oscillator::setSampleRate(float sampleRate) {
  mSampleRate = sampleRate;
  mIncrementFactor = mWaveTableSize / mSampleRate;
  //updateIncrement();
}

void Oscillator::updateIncrement(float frequency) {
  mPhaseIncrement = frequency * mIncrementFactor;
  lPhaseIncrement = long(mPhaseIncrement * 65536.0);
  
  mOffPhaseIncrement = mPhaseIncrement * mSawXfactor;
  lOffPhaseIncrement = long(mOffPhaseIncrement * 65536.0);
  
  mSubPhaseIncrement = mPhaseIncrement * mSubFactor;
  lSubPhaseIncrement = long(mSubPhaseIncrement * 65536.0);
}


float Oscillator::getSawSample(long phase, long inc){
  float ft;
  float sample;
  int blep;
  sample = mSawWaveTable[int(phase >> 16)]; 
    //
    //Uncomment the return below to skip Anti-Alyasing correction.
    // return sample;
    if(phase < inc){
    //  sample = sample - poly_Blep_Fall(phase, inc);
      
      blep = (phase << 7 ) / inc;
      //return sample - blepFallingTable[blep];
      
      //PolyBlep - comment the return above to execute PolyBlep
      ft = float(phase) / float(inc);
      return (sample - ((ft + ft) - (ft * ft) - 1.0));
     
    }
    else if(phase > (lMaxPhase - inc)){
    //  sample = sample - poly_Blep_Rise(phase, inc);
      
      blep = ((phase - lMaxPhase) << 7 ) / inc;
      //return sample - blepRisingTable[blep];
      
      //PolyBlep - comment the return above to execute PolyBlep
      ft = (float(phase) - fMaxPhase) / float(inc);
      return (sample - ((ft * ft) + (ft + ft) + 1.0));
    } 
  return sample;
}
  

float Oscillator::getSawSamplePolyBlep(long phase, long inc){
  float ft;
  float sample;
  sample = mSawWaveTable[int(phase >> 16)];
    if(phase < inc){
      //sample = sample - poly_Blep_Fall(phase, inc);
      ft = float(phase) / float(inc);
      return (sample - ((ft + ft) - (ft * ft) - 1.0));
    }
    else if(phase > lMaxPhase - inc){
      //sample = sample - poly_Blep_Rise(phase, inc);
      ft = (float(phase) - fMaxPhase) / float(inc);
      return (sample - ((ft * ft) + (ft + ft) + 1.0));
    } 
  return sample;
  //return FixedPoint::convertToFP(sample);
}

float Oscillator::getNaiveSample() {
  lPhase += lPhaseIncrement;
    if(lPhase >= lWaveTableSize){
      lPhase = long(lPhase - lWaveTableSize);
    }
  switch (mWaveform) {
    case SINE:
      mNextSample = mSineWaveTable[int(lPhase >> 16)];
      break;
    case SAW:;
      mNextSample = mSawWaveTable[int(lPhase >> 16)];
      break;
    case SQUARE_PULSE:
     if(lPhase >= lPulseThreshold){
          mNextSample = -kx;
     }
     else{
          mNextSample = kx;
     }
     break;
  }
  return (mNextSample);
}

float Oscillator::getSample() {
  float phase2;
  lPhase += lPhaseIncrement;
    if(lPhase >= lWaveTableSize){
      lPhase = long(lPhase - lWaveTableSize);
    }
  switch (mWaveform) {
    case SINE:
      mNextSample = mSineWaveTable[int(lPhase >> 16)];
      break;
    case SAW:
      mNextSample = getSawSample(lPhase, lPhaseIncrement);
      break;
    case SQUARE_PULSE:
      if(lPhase >= lWaveTableSize){
        lPhase = long(lPhase - lWaveTableSize);
      }
      phase2 = lPhase + lPulseThreshold;
      if(phase2 >= lWaveTableSize){
        phase2 = phase2 - lWaveTableSize;
      }
     mNextSample = (getSawSample(lPhase, lPhaseIncrement) - getSawSample(phase2, lPhaseIncrement)) - mPulseOffset;
     break;
  }
  return (mNextSample);
}

  void Oscillator::updateNaiveBuffer(DoubleBuffer buffer){
  switch (mWaveform) {
    case SINE:
      while(buffer.isReadyToWrite()){
        lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }
        buffer.write(mSineWaveTable[int(lPhase >> 16)]);
      }
      break;
    case SAW:
      while(buffer.isReadyToWrite()){
        lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }
        //Without Anti-Aliasing PolyBlep correction
        buffer.write(mSawWaveTable[int(lPhase >> 16)]);
      }
      break;
    case SQUARE_PULSE:
      //for(int i = 0; i < nSamples; i++){
      while(buffer.isReadyToWrite()){
       lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }
        //Without Anti-Aliasing PolyBlep correction
        if(lPhase >= lPulseThreshold){
          buffer.write(-kx);
        }
        else{
          buffer.write(kx);
        }
      }
      break;
    }
  }
    
 void Oscillator::updateBuffer(DoubleBuffer buffer){
  float saw1, saw2, saw3, saw4;
  float saw, pulse;
  //long saw1, saw2, saw3, saw4;
  //long saw, pulse;
  long phase1, phase2, phase3, phase4, phase5;
  float* writeBuffer;
  int nSamples;
  switch (mWaveform) {
    case SINE:
      while(buffer.isReadyToWrite()){
        lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }
        buffer.write(mSineWaveTable[int(lPhase >> 16)]);
      }
      break;
    case SAW:
      while(buffer.isReadyToWrite()){
        lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }
        //With Anti-Aliasing PolyBlep correction
        //saw = getSawSample(lPhase, lPhaseIncrement;
        buffer.write(getSawSample(lPhase, lPhaseIncrement));
        //buffer.write(FixedPoint::convertToFP(getSawSample(lPhase, lPhaseIncrement)));
        //Without Anti-Aliasing PolyBlep correction
        //buffer.write(mSawWaveTable[int(lPhase >> 16)]);
      }
      break;
    case SQUARE_PULSE:
      while(buffer.isReadyToWrite()){
       lPhase += lPhaseIncrement;
        if(lPhase >= lWaveTableSize){
          lPhase = long(lPhase - lWaveTableSize);
        }        
        ///*
        //---------------------------------------------------------
        //This code is used when SQUARE-PULSE is derived from SAW
        //With Anti-Aliasing PolyBlep correction
        //---------------------------------------------------------
        phase1 = lPhase;
        phase2 = lPhase + lPulseThreshold;
        
        if(phase2 >= lWaveTableSize){
          phase2 = phase2 - lWaveTableSize;
        }
        ///*
        saw1 = getSawSample(phase1, lPhaseIncrement);
        saw2 = getSawSample(phase2, lPhaseIncrement);
        pulse = (saw1 - saw2) - mPulseOffset;
        buffer.write(pulse);
        //buffer.write(FixedPoint::convertToFP(pulse));
        //*/
        /*
        saw1 = FixedPoint::convertToFP(getSawSample(phase1, lPhaseIncrement));
        saw2 = FixedPoint::convertToFP(getSawSample(phase2, lPhaseIncrement));
        pulse = (saw1 - saw2) - lPulseOffset;
        buffer.write(pulse);
        */
      }
      break;
    case XTREME:
      //for(int i = 0; i < nSamples; i++){
      while(buffer.isReadyToWrite()){
       lPhase += lPhaseIncrement;
       if(lPhase >= lWaveTableSize){
         lPhase = long(lPhase - lWaveTableSize);
       }  
       //Update Offtune Oscillator
       lOffPhase += lOffPhaseIncrement;
       if(lOffPhase >= lWaveTableSize){
          lOffPhase = long(lOffPhase - lWaveTableSize);
       }
       //Update Sub Oscillator
       lSubPhase += lSubPhaseIncrement;
       if(lSubPhase >= lWaveTableSize){
          lSubPhase = long(lSubPhase - lWaveTableSize);
       } 
        ///*
        //---------------------------------------------------------
        //This code is used when SQUARE-PULSE is derived from SAW
        //With Anti-Aliasing PolyBlep correction
        //---------------------------------------------------------
        phase1 = lPhase;
        phase2 = lPhase + lPulseThreshold;
        if(phase2 >= lWaveTableSize){
          phase2 = phase2 - lWaveTableSize;
        }
       
        saw1 = getSawSample(phase1, lPhaseIncrement);
        saw2 = getSawSample(phase2, lPhaseIncrement);
       
        saw3 = getSawSample(lOffPhase, lOffPhaseIncrement);
        saw4 = getSawSample(lSubPhase, lSubPhaseIncrement);
        
        saw = saw1 + saw3;
        pulse = (saw1 - saw2) - mPulseOffset;
  
        buffer.write(mSawAmount * saw + mPulseAmount * pulse + mSubAmount * saw4);
        
      }
      break;
    }
 }
