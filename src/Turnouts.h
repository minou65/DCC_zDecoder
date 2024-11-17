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

enum class TurnoutDirection {
	Closed,
	Thrown
};

class UnCoupler : public accessories {
public:
	UnCoupler() = default;
	UnCoupler(int8_t Port);
	UnCoupler(int8_t Port, int8_t Address);
	UnCoupler(int8_t Port, int8_t Address, uint16_t PulsTime);
	~UnCoupler();
	AccessoryType getType() const;
	void notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower);
	void process();
	void on();
	void Thrown();
	void off();

	void Closed();

private:
	CoilPulsed _coil;
	TurnoutDirection _direction;
	
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
	AccessoryType getType() const;
	void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	void process();
	void on();  // Turnout
	void off();  // Closed

private:
	TurnoutDirection _direction;

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

