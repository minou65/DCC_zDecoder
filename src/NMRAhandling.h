// NMRAhandling.h

#ifndef _NMRAHANDLING_h
#define _NMRAHANDLING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <NmraDcc.h>

// DCC ACK for Service Mode Programming Read CV Capablilty 
#define DccAckPin  13  // This is Port D13 

#define DCC_DECODER_VERSION_NUM 1  // Set the Decoder Version - Used by JMRI to Identify the decoder
#define DCC_Default_Addr 3

#define CV_DECODER_UNLOCK	15		// Decoder unlock, muss den selben Wert haben wie CV_DECODER_CARRIER
#define CV_DECODER_CARRIER	16

#define CV_DCC_POM_PROG		44		// CV 44 beinhaltet LSB der Digitaladresse, unter der der Decoder auf 
// dem Hauptgleis programmiert werden kann (POM)

#define BASE_CV_Mode 520

#define BASE_CV_LSB_ 540
#define BASE_CV_MSB_ 560

#define BASE_CV_onTime_ 580
#define BASE_CV_offTime_ 600
#define BASE_CV_Multiplikator_ 620

void NMRAsetup();
void NMRAloop();

#endif

