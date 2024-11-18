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
public:
	Turnout() = default;
	Turnout(int8_t ThrownPort, int8_t ClosedPort);
	Turnout(int8_t ThrownPort, int8_t ClosedPort, uint16_t Address);
	Turnout(int8_t ThrownPort, int8_t ClosedPort, uint16_t Address, uint16_t PulsTime);
	~Turnout();
	AccessoryType getType() const;
	void notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower);
	void process();
	void on();  // Turnout
	void Thrown();
	void off();  // Closed

	void Closed();

private:
	TurnoutDirection _direction;
	CoilPulsed _coil1;
	CoilPulsed _coil2;

};

class TurnoutServo : public accessories {
public:
	TurnoutServo() = default;
	TurnoutServo(int8_t ServoPort, uint16_t Address, int16_t limit1, int16_t limit2, int16_t travelTime);
	~TurnoutServo();
	void notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower);
	void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);
	void process();
	void on();
	void off();
private:
	ServoControl _servo;
	TurnoutDirection _direction;

	void MoveServo(uint16_t percentage, bool clockwise);
};

#endif

