// DecoderCoreConfig.h

#ifndef _DecoderCoreConfig_h
#define _DecoderCoreConfig_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define SOUND_FEATURE 0 // 0 = Sound disabled, 1 = Sound enabled
#define INPUT_PIN_FEATURE 0 // 0 = Input pin disabled, 1 = Input pin enabled


#define TRAFFIC_SIGNAL_FEATURE 0 // 0 = Traffic signal disabled, 1 = Traffic signal enabled

#define NRMA_DCC_FEATURE 1 // 0 = NMRA disabled, 1 = NMRA enabled

#endif

