//-------------------------------------------------------------------------------------
//SynthUtilities.cpp - Last update: 29 Mar 2015 - Started: 26 Feb 2015.
//Contains a number of classes used in My Synth
//Implementes double buffer mechanism for float numbers.
//-------------------------------------------------------------------------------------
//29 Mar 2015: Implemented LFO and Mod Wheel Cutoff Modulation and improved Cutoff 
//Modulation in general.
//-------------------------------------------------------------------------------------
//06 Mar 2015: Included in SythUtilities namespace FixedPoint to provide the operations
//to support Fixed Point Integer calculation.
//-----------------------------------------------------------
//02 Mar 2015: Implemented class DoubleBuffer in SynthUtilities and it is partially working.
//There is some problem with the logic to check if the Write Buffer is Ready to Write.
//It is working using part of the original logic without the Double Buffer Class.
//-----------------------------------------------------------

#include "SynthUtilities.h"

long FixedPoint::multiply(long l1, long l2){
  return long((int64_t(l1) * int64_t(l2)) >> FIXED_FBITS);
}

long  FixedPoint::convertToFP(float x){
  return long(x * floatFactor);
}

float  FixedPoint::convertToFloat(long x){
  return float(float(x) / floatFactor);
}

DoubleBuffer::DoubleBuffer(){
  start(128);
}

DoubleBuffer::DoubleBuffer(int bufferSize){
   start(bufferSize);
}

void DoubleBuffer::start(int bufferSize){
  if(bufferSize != bSize){
    bSize = bufferSize;
    buffer0 = new float[bufferSize];
    buffer1 = new float[bufferSize];
  }
  writeBuffer = buffer0;
  readBuffer = buffer1;
  for(int i=0; i<bufferSize; i++){
    buffer0[i] = 0.0;
    buffer1[i] = 0.0;
  }
  readSample = 0;
  writeSample = 0;
  readyToWrite = FALSE;
  swapBuffer();
}

void DoubleBuffer::synch(){
  writeSample = 0;
  readyToWrite = TRUE;
}

int DoubleBuffer::getBufferSize(){
  return bSize;
}

float* DoubleBuffer::getWriteBuffer(){
  return writeBuffer;
}

void DoubleBuffer::releaseWriteBuffer(){
  readyToWrite = FALSE;
}

void DoubleBuffer::retainWriteBuffer(){
  readyToWrite = TRUE;
}

float* DoubleBuffer::getReadBuffer(){
  return readBuffer;
}

void DoubleBuffer::swapBuffer(){
  //Serial.println("Swap Buffer");
  float* xBuffer;
  xBuffer = readBuffer;
  readBuffer = writeBuffer;
  writeBuffer = xBuffer;
  readSample = 0;
  writeSample = 0;
  readyToWrite = TRUE;
}

float DoubleBuffer::read(){
  float* xBuffer;
  readSample++;
  if(readSample >= bSize){
    swapBuffer();
  }
  return (readBuffer[readSample]);
}

void DoubleBuffer::write(float sample){
  //Serial.println("Write Buffer");
  writeBuffer[writeSample] = sample;
  writeSample++;
  if(writeSample >= bSize){
    readyToWrite = FALSE;
  }
}

int DoubleBuffer::isReadyToWrite(){
  return readyToWrite; 
}


