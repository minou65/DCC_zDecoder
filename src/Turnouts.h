// Turnouts.h

#ifndef _TURNOUTS_h
#define _TURNOUTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "CoilControl.h"
#include "ServoControl.h"

class UnCoupler : public accessories {
protected:
	CoilPulsed coil;

public:
	UnCoupler() = default;
	UnCoupler(int8_t Port_);
	UnCoupler(int8_t Port_, int8_t Address_);
	UnCoupler(int8_t Port_, int8_t Address_, uint16_t PulsTime_);
	~UnCoupler();
	void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	void process();
	void on();
	void off();
};

class Turnout : public accessories {
protected:
	CoilPulsed coil1;
	CoilPulsed coil2;

public:
	Turnout() = default;
	Turnout(int8_t RPort_, int8_t GPort_);
	Turnout(int8_t RPort_, int8_t GPort_, uint16_t Address_);
	Turnout(int8_t RPort_, int8_t GPort_, uint16_t Address_, uint16_t PulsTime_);
	~Turnout();
	void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	void process();
	void on();  // Turnout
	void off();  // Closed

};

class TurnoutServo : public accessories {
protected:
	ServoControl servo;

	void MoveServo(uint16_t percentage_, bool clockwise_);

public:
	TurnoutServo() = default;
	TurnoutServo(int8_t ServoPort_, uint16_t Address_, int16_t limit1_, int16_t limit2_, int16_t travelTime_);
	~TurnoutServo();
	void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);
	void process();
	void on();
	void off();
};

#endif

