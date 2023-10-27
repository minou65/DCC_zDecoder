
#ifndef _COILS_h
#define _COILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "neotimer.h"

class Coil {
protected:
	const int8_t GPIO;
	bool IsActive;
	
public:
	Coil();
	Coil(const int8_t GPIO_);
	~Coil();

	// Muss regelmässig aufgerufen werden
	virtual void process();

	void on();
	void off();
	bool isOn();
};

class CoilPulsed : public Coil {
protected:
	Neotimer Puls;

	int8_t PulsLength_;

public:
	CoilPulsed(const int8_t GPIO_);
	CoilPulsed(const int8_t GPIO_, const uint16_t PulsLength_);
	~CoilPulsed();

	// Muss regelmässig aufgerufen werden
	virtual void process();

	void on();
	void off();
};

#endif

