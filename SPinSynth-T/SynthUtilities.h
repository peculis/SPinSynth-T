//-------------------------------------------------------------------------------------
//SynthUtilities.h - Last update: 29 Mar 2015 - Started: 26 Feb 2015.
//Contains a number of classes used in My Synth
//Implements the double buffer mechanism for float numbers.
//-------------------------------------------------------------------------------------
//29 Mar 2015: Created KeyEvent class (behaves like a record) to manage Key ON/OFF events.
//This class is used to replicate the behaviour of monophonic keyboards where when two notes 
//are pressed simultaneously the higher note will play and when the highest key is released 
//the higher key immediately below that is still pressed will play.
//-------------------------------------------------------------------------------------
//06 Mar 2015: Included in SynthUtilities namespace FixedPoint to provide the operations
//to support Fixed Point Integer calculation.
//-----------------------------------------------------------
//05 Mar 2015: Included some constants to handle fixed point long integer calculations.
//-----------------------------------------------------------
//02 Mar 2015: Implemented class DoubleBuffer in SynthUtilities and it is partially working.
//There is some problem with the logic to check if the Write Buffer is Ready to Write.
//It is working using part of the original logic without the Double Buffer Class.
//-----------------------------------------------------------

#ifndef SYNTH_UTILITIES_H
#define SYNTH_UTILITIES_H

#include <Arduino.h>
#include <stdint.h>

#ifndef TRUE_FALSE
#define TRUE_FALSE
#define FALSE 0
#define TRUE 1
#endif

#ifndef ON_OFF
#define ON_OFF
#define OFF 0
#define ON 1
#endif

#ifndef LED
#define LED 13
#endif

#define FIXED_BITS        32
#define FIXED_WBITS       18
#define FIXED_FBITS       14
#define FIXED_TO_INT(a)   (int32_t(a) >> FIXED_FBITS)
#define FIXED_FROM_INT(a) (int32_t)((a) << FIXED_FBITS)
#define FIXED_MAKE(a)     (int32_t)((a*(1 << FIXED_FBITS)))

const int64_t fpDivisionFactor = 1 << FIXED_FBITS;
const int32_t fpMask = fpDivisionFactor -1;
const float floatFactor = float(1 << FIXED_FBITS);

namespace FixedPoint{
  long multiply(long l1, long l2);
  long convertToFP(float x);
  float convertToFloat(long x);
}

class KeyEvent{
  public:
  int pitch = 0;
  int state = OFF;
  int velocity = 0;
  int age = 25;
};

class DoubleBuffer {
  
public:
  DoubleBuffer();
  DoubleBuffer(int bufferSize);
  void start(int bufferSize);
  int getBufferSize();
  float* getWriteBuffer();
  void releaseWriteBuffer();
  void retainWriteBuffer();
  float* getReadBuffer();
  float read();
  void write(float sample);
  int isReadyToWrite(); 
  void swapBuffer();
  void synch();
  
private:
  int bSize;
  float* buffer0;
  float* buffer1;
  float* readBuffer;
  float* writeBuffer;
  int readSample = 0;
  int writeSample = 0;
  int readyToWrite = FALSE;
};


#endif
