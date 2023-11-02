// Watchdog.h

#ifndef _WATCHDOG_h
#define _WATCHDOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "neotimer.h"
#include "CoilControl.h"

class Watchdog : public accessories {
private:
	Neotimer timer;
	uint16_t Interval;

	Coil Output;
public:
	Watchdog(uint16_t BaseAddress_, byte BaseChannel_, uint16_t Interval_);
	~Watchdog();

	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();
};


#endif

