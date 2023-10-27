// Road_Signals.h

#ifndef _ROAD_SIGNALS_h
#define _ROAD_SIGNALS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "LEDControl.h"
#include "neotimer.h"

class Road_Signal_70 : public Signal {
private:
	LEDFader Red;		// rot
	LEDFader Orange;		// orange
	LEDFader Green;		// grün


	Neotimer OperationTimer;

public:
	Road_Signal_70(uint16_t BaseAddress_, byte BaseChannel_,
					uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, 
					byte fadeUpTime_, byte fadeDownTime_, byte Dealy_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();
};


#endif

